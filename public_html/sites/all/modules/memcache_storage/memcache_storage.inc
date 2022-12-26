<?php

/**
 * @file
 * Provides class for memcache data handling.
 */

// Load include with memcache manager class.
require_once dirname(__FILE__) . '/memcache_storage.manager.inc';

/**
 * Class handles memcache data.
 */
class MemcacheStorage implements DrupalCacheInterface {

  protected $bin;
  protected $memcache;
  protected $compressed;
  protected $debug_mode;
  protected $debug_timer;

  /**
   * Constructs a new MemcacheStorage object.
   */
  function __construct($bin) {

    // Save cache bin as object prorerty.
    $this->bin = $bin;

    // Set compress data mode.
    $this->compressed = variable_get('memcache_storage_compress_data', 0);

    // Set debug mode.
    $this->debug_mode = variable_get('memcache_storage_debug', FALSE);

    // Initialize debug timer counter.
    $this->debug_timer = 0;

    // Connect to memcache.
    $this->memcache = MemcacheStorageManager::loadObject($this->bin);
  }

  /**
   * Destructs MemcacheStorage object.
   * Closes memcache connection.
   */
  function __destruct() {
    MemcacheStorageManager::destroyObject($this->bin);
  }

  /**
   * Implements DrupalCacheInterface::get().
   */
  function get($cid) {
    $cids = array($cid);
    $cache = $this->getMultiple($cids);
    return isset($cache[$cid]) ? $cache[$cid] : FALSE;
  }

  /**
   * Implements DrupalCacheInterface::getMultiple().
   */
  function getMultiple(&$cids) {

    // No memcache connection.
    if (empty($this->memcache)) {
      return FALSE;
    }

    $cache = $this->_memcache_get($cids);

    // Leave cache ids that was unable to get data from memcache.
    $cids = array_diff($cids, array_keys($cache));

    return $cache;
  }

  /**
   * Implements DrupalCacheInterface::set().
   */
  function set($cid, $data, $expire = CACHE_PERMANENT) {

    // No memcache connection.
    if (empty($this->memcache)) {
      return FALSE;
    }

    // Build cache object (as Drupal always do).
    $cache = new stdClass;
    $cache->cid = $cid;
    $cache->data = $data;
    $cache->created = REQUEST_TIME;
    $cache->expire = $expire;

    // We should always keep in storage permanent and temporary cache data.
    // Such cached data are invalidates during cache_get() operations.
    if ($expire == CACHE_PERMANENT || $expire == CACHE_TEMPORARY) {
      $expire = 0;
    }
    // Otherwise simply convert default drupal expire time to memcache expire time.
    elseif ($expire > REQUEST_TIME) {
      $expire -= REQUEST_TIME;
    }

    // Save data in memcache pool.
    $this->_memcache_set($cid, $cache, $expire);
  }

  /**
   * Implements DrupalCacheInterface::clear().
   */
  function clear($cid = NULL, $wildcard = FALSE) {

    // No memcache connection.
    if (empty($this->memcache)) {
      return FALSE;
    }

    // Function like cache_clear_all(NULL, 'cache_bin');
    // We should invalidate all old cache.
    if (empty($cid)) {

      $cache_min_lifetime = variable_get('cache_lifetime', 0);
      if (!empty($cache_min_lifetime)) {
        // Load timestamp of last cache flush for this bin.
        $cache_last_flush = variable_get('cache_flush_' . $this->bin, 0);
        if (empty($cache_last_flush) || REQUEST_TIME > ($cache_last_flush + $cache_min_lifetime)) {
          // Set timestamp that memcache bin was flushed.
          // Actually, before cache load we will check this param.
          $this->variableSet('cache_flush_' . $this->bin, REQUEST_TIME);
        }
      }
      else {
        // No minimum cache lifetime, flush all temporary cache entries now.
        $this->variableSet('cache_flush_' . $this->bin, REQUEST_TIME);
      }
    }
    else {
      if ($wildcard) {
        if ($cid == '*') {
          // Simply change hash for bin storage will flushes all bin (as it used in building memcache key).
          // @see MemcacheStorage::buildCacheKey();
          $this->memcache->increment($this->bin);
        }
        else {

          // Load array with all wildcards.
          $wildcards = variable_get('memcache_storage_wildcards', array());

          // Add / replace wildcard.
          // Every cache item check wildcard matches after load from memcache.
          $wildcards[$this->bin][$cid] = REQUEST_TIME;
          $this->variableSet('memcache_storage_wildcards', $wildcards);
        }
      }
      elseif (is_array($cid)) {
        foreach ($cid as $cache_id) {
          $this->_memcache_delete($cache_id);
        }
      }
      else {

        // variable_set() and variable_del() are one of the most often used functions in Drupal.
        // Every time they calls variables cache should be deleted, and will be generated only during next bootstrap.
        // for example, if page contains 20 variable_set() calls, it will delete value from memcache only once,
        // all other 19 delete queries will do extra job. So, to avoid this useless queries I suggested to check whether variables
        // cache was already deleted and skip memcache_delete if so.
        if ($cid == 'variables' && $this->bin == 'cache_bootstrap') {
          $variables_cache_flushed = &drupal_static('variables_cache_flushed');
          if ($variables_cache_flushed) {
            // It was already flushed.
            // So prevent memcache query and return positive result.
            return TRUE;
          }
          $variables_cache_flushed = TRUE;
        }

        $this->_memcache_delete($cid);
      }
    }

  }

  /**
   * Implements DrupalCacheInterface::isEmpty().
   */
  function isEmpty() {
    // It is unable to get empty state of memcache pool,
    // so we assume it never empty.
    return FALSE;
  }

  /**
   * Get values from memcache storage.
   * Provide debug logging.
   */
  private function _memcache_get($cids) {

    /* START DEBUG. FIRST PHASE. */
    if ($this->debug_mode) {
      // Start count time spent on getting data from memcache.
      $timer = $this->bin . '_' . $this->debug_timer;
      timer_start($timer);
      // Get memory usage before set new value into cache.
      $memory_usage_start = memory_get_usage();
    }
    /* STOP DEBUG. END OF FIRST PHASE. */

    // Generate unique memcache key for every cid.
    $cache_ids = array();
    foreach ($cids as $cid) {
      $cache_ids[] = $this->buildCacheKey($cid);
    }

    // Get data from memcache storage.
    $data = $this->memcache->get($cache_ids);

    // Build a new array with cached data using normal cache id.
    $cache = array();
    if ($data !== FALSE) {
      foreach ($data as $item) {
        // Check expiration for every cache item recieved from memcache.
        if ($item = $this->validateItem($item)) {
          $cache[$item->cid] = $item;
        }
      }
    }

    /* START DEBUG. LAST PHASE. */
    if ($this->debug_mode) {

      // Stop count timer.
      timer_stop($timer);

      // Calculate memory usage.
      $used_memory = memory_get_usage() - $memory_usage_start;

      // Get array with debug data.
      $memcache_storage_debug_output = &drupal_static('memcache_storage_debug_output');

      // Log entry about memcache action.
      foreach ($cids as $cid) {
        $memcache_storage_debug_output[] = array(
          'action' => count($cids) > 1 ? 'getMultiple' : 'get',
          'timer' => timer_read($timer),
          'memory' => number_format($used_memory / 1024, 2),
          'bin' => $this->bin,
          'key' => $cid,
          'result' => isset($cache[$cid]) ? 'HIT' : 'MISS',
        );
      }

      // Increase timer to get new time results.
      $this->debug_timer++;
    }
    /* END DEBUG. LAST PHASE. */

    return $cache;
  }

  /**
   * Set data to memcache pool.
   * Provide debug logging.
   */
  private function _memcache_set($cache_id, $data, $expire) {

    /* START DEBUG. FIRST PHASE. */
    if ($this->debug_mode) {
      // Start count time spent on setting data to memcache.
      $timer = $this->bin . '_' . $this->debug_timer;
      timer_start($timer);
      // Get memory usage before set new value into cache.
      $memory_usage_start = memory_get_usage();
    }
    /* STOP DEBUG. END OF FIRST PHASE. */

    // Build unique memcache key.
    $key = $this->buildCacheKey($cache_id);

    // Add data to memcache pool.
    $result = $this->memcache->set($key, $data, $this->compressed, $expire);

    /* START DEBUG. LAST PHASE. */
    if ($this->debug_mode) {

      // Stop count timer.
      timer_stop($timer);

      // Calculate memory usage.
      $used_memory = memory_get_usage() - $memory_usage_start;

      // Get array with debug data.
      $memcache_storage_debug_output = &drupal_static('memcache_storage_debug_output');

      // Log entry about memcache action.
      $memcache_storage_debug_output[] = array(
        'action' => 'set',
        'timer' => timer_read($timer),
        'memory' => $used_memory,
        'bin' => $this->bin,
        'key' => $cache_id,
        'result' => $result ? 'HIT' : 'MISS',
      );

      // Increase timer to get new time results.
      $this->debug_timer++;
    }
    /* END DEBUG. LAST PHASE. */
  }

  /**
   * Delete value from memcache pool.
   * Provide debug logging.
   */
  private function _memcache_delete($cache_id) {

    /* START DEBUG. FIRST PHASE. */
    if ($this->debug_mode) {
      // Start count time spent on setting data to memcache.
      $timer = $this->bin . '_' . $this->debug_timer;
      timer_start($timer);
      // Get memory usage before set new value into cache.
      $memory_usage_start = memory_get_usage();
    }
    /* STOP DEBUG. END OF FIRST PHASE. */

    $key = $this->buildCacheKey($cache_id);
    $result = $this->memcache->delete($key);

    /* START DEBUG. LAST PHASE. */
    if ($this->debug_mode) {

      // Stop count timer.
      timer_stop($timer);

      // Calculate memory usage.
      $used_memory = memory_get_usage() - $memory_usage_start;

      // Get array with debug data.
      $memcache_storage_debug_output = &drupal_static('memcache_storage_debug_output');

      // Log entry about memcache action.
      $memcache_storage_debug_output[] = array(
        'action' => 'delete',
        'timer' => timer_read($timer),
        'memory' => $used_memory,
        'bin' => $this->bin,
        'key' => $cache_id,
        'result' => $result ? 'HIT' : 'MISS',
      );

      // Increase timer to get new time results.
      $this->debug_timer++;
    }
    /* END DEBUG. LAST PHASE. */
  }

  /**
   * Validates cache item.
   * Checks if it is still valid and not expired.
   *
   * @param $cache
   *   Cache item loaded from memcache pool.
   *
   * @return bool|object
   *   Return FALSE if object is not valid.
   *   Return cache item otherwise.
   */
  protected function validateItem($cache) {

    // Load array with all wildcards.
    $wildcards = variable_get('memcache_storage_wildcards', array());
    $current_wildcards = !empty($wildcards[$this->bin]) ? $wildcards[$this->bin] : array();

    foreach ($current_wildcards as $wildcard => $flush_timestamp) {

      // See if wildcard is actual for current cache item.
      if ($cache->created < $flush_timestamp) {

        // See if current cache id matches wildcard.
        if (strpos($cache->cid, $wildcard) === 0) {

          // Remove expired item from memcache.
          $this->_memcache_delete($cache->cid);

          // Return no value from cache.
          return FALSE;
        }
      }
    }

    // For temporary cache we should check last bin flush.
    // If temporary cache was created earlier than last cache flush - cache is invalid.
    if ($cache->expire == CACHE_TEMPORARY) {
      if ($cache->created < variable_get('cache_flush_' . $this->bin, 0)) {

        // Remove expired item from memcache.
        $this->_memcache_delete($cache->cid);

        // Return no value from cache.
        return FALSE;
      }
    }

    return $cache;
  }

  /**
   * Generates unique cache key.
   *
   * @param $cid
   *   Cache ID name.
   *
   * @return string
   *   Unique cache key.
   */
  protected function buildCacheKey($cid) {

    // Unique hash of bin storage.
    // To flush cache for single storage we simply need to change this value.
    $bin_hash = &drupal_static('memcache_storage_bin_hash_' . $this->bin);
    if (empty($bin_hash)) {
      $bin_hash = $this->memcache->get($this->bin);
      if (empty($bin_hash)) {
        $this->memcache->set($this->bin, $bin_hash = 1, $this->compressed, 0);;
      }
    }

    // Load key prefix from settings.php.
    $key_prefix = variable_get('memcache_storage_key_prefix', '');

    // Build unique cache key.
    $key_base = $key_prefix . '-' . $this->bin . '_' . $bin_hash . '-';
    $full_key = $key_base . '-' . $cid;

    // Memcache only supports key lengths up to 250 bytes.  If we have generated
    // a longer key, hash it with sha1 which will shrink the key down to 40 bytes
    // while still keeping it unique.
    if (strlen($full_key) > 250) {
      $full_key = $key_base . '-' . sha1($cid);
    }

    return $full_key;
  }

  /**
   * Internal function for setting new variable's value.
   * It skips two extra steps of variable_set() function:
   *   1. Avoid execution of cache_clear_all('variables', 'cache_bootstrap').
   *   2. Avoid extra load of `variable` table on next Drupal bootstrap.
   */
  protected function variableSet($name, $value) {
    global $conf;

    // If the name already matches the value, ignore the variable set.
    if (isset($conf[$name]) && $conf[$name] === $value) {
      return;
    }

    db_merge('variable')
      ->key(array('name' => $name))
      ->fields(array('value' => serialize($value)))
      ->execute();

    // If the variables are cached, get a fresh copy, update with the new value
    // and set it again.
    if ($cached = cache_get('variables', 'cache_bootstrap')) {
      $variables = $cached->data;
      $variables[$name] = $value;
      cache_set('variables', $variables, 'cache_bootstrap');
    }

    $conf[$name] = $value;
  }

}
