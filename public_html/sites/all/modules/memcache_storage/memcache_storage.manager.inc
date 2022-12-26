<?php

/**
 * @file
 * Provide class that manages memcache connections.
 */

/**
 * Manage memcache server(s) connections.
 */
class MemcacheStorageManager {

  private static $servers;

  public static function loadObject($bin) {

    // Get name of cluster for current cache bin storage.
    $cluster = MemcacheStorageManager::getCacheBinCluster($bin);

    if (empty(MemcacheStorageManager::$servers[$cluster])) {

      // Load available servers from settings.php.
      $server_list = variable_get('memcache_storage_servers', array('default' => '127.0.0.1:11211'));

      // Load server that matches current cache bin.
      if (isset($server_list[$cluster])) {
        $server = $server_list[$cluster];
      }
      else {

        // Log error.
        MemcacheStorageManager::logError('Could not connect to cluster %cluster for bin %bin.',
          array('%cluster' => $cluster, '%bin' => $bin));

        // Return not initialized value.
        return FALSE;
      }

      list($host, $port) = explode(':', $server);

      // Support unix sockets in the format 'unix:///path/to/socket'.
      if ($host == 'unix') {
        // When using unix sockets use the full path for $host.
        $host = $server;
        // Port is always 0 for unix sockets.
        $port = 0;
      }

      // Trying connect to memcache.
      $memcache_obj = new Memcache;
      $connected = @$memcache_obj->connect($host, $port);

      if ($connected) {

        // Provided compressThreshold support.
        // For more information see http://www.php.net/manual/en/memcache.setcompressthreshold.php.
        $compress_threshold = variable_get('memcache_storage_compress_threshold', array('threshold' => 20000, 'min_savings' => 0.2));
        if (isset($compress_threshold['threshold']) && isset($compress_threshold['min_savings'])) {
          $memcache_obj->setCompressThreshold($compress_threshold['threshold'], $compress_threshold['min_savings']);
        }

        MemcacheStorageManager::$servers[$cluster] = $memcache_obj;
      }
      else {

        // Log error.
        MemcacheStorageManager::logError('Could not connect to server %server (port %port).',
          array('%server' => $host, '%port' => $port));

        // Return not initialized value.
        return FALSE;
      }
    }

    return MemcacheStorageManager::$servers[$cluster];
  }

  static public function destroyObject($bin) {

    // Get name of cluster for current cache bin storage.
    $cluster = MemcacheStorageManager::getCacheBinCluster($bin);

    // Close memcache connection and destroy Memcache object.
    if (!empty(MemcacheStorageManager::$servers[$cluster])) {
      MemcacheStorageManager::$servers[$cluster]->close();
      unset(MemcacheStorageManager::$servers[$cluster]);
    }
  }

  static public function flushClusters() {

    $server_list = variable_get('memcache_storage_servers', array('default' => '127.0.0.1:11211'));
    foreach ($server_list as $server) {

      list($host, $port) = explode(':', $server);

      // Support unix sockets in the format 'unix:///path/to/socket'.
      if ($host == 'unix') {
        // When using unix sockets use the full path for $host.
        $host = $server;
        // Port is always 0 for unix sockets.
        $port = 0;
      }

      $memcache = new Memcache();
      $connected = $memcache->connect($host, $port);
      if ($connected) {
        $memcache->flush();
        $memcache->close();
      }
      else {
        MemcacheStorageManager::logError('Could not connect to server %server (port %port).',
          array('%server' => $host, '%port' => $port));
      }
    }

  }

  static public function getCacheBinCluster($bin) {

    // Example:
    // $conf['memcache_storage_bins'] = array(
    //   'cache' => 'default',
    //   'cache_page' => 'pages',
    // );
    $cluster_bins = variable_get('memcache_storage_bins', array());

    // Get server name for current cache bin.
    $cluster = 'default';
    if (!empty($cluster_bins[$bin])) {
      $cluster = $cluster_bins[$bin];
    }

    return $cluster;
  }

  static private function logError($message, $variables = array()) {
    watchdog('memcache_storage', $message, $variables, WATCHDOG_CRITICAL);
  }

}
