==================
## REQUIREMENTS ##
==================

- PHP 5.1 or greater
- Availability of a memcached daemon: http://memcached.org/
- PECL memcache package: http://pecl.php.net/package/memcache (version 2.2.1 or greater).

==================
## INSTALLATION ##
==================

These are the broad steps you need to take in order to use this software. Order
is important.

 1. Install the memcached binaries on your server. See for instance: http://www.lullabot.com/articles/how_install_memcache_debian_etch

 2. Install the PECL memcache extension for PHP. This must be version 2.2.1 or higher or you will experience errors.

 3. Start at least one instance of memcached on your server.

 4. Put your site into offline mode.

 5. Download and extract memcache_storage module into sites/all/modules directory.

 6. Edit settings.php to configure Memcache Storage module (see ## CONFIGURATIONS ## section).

 7. Install the memcache_storage module as usual.

 8. Bring your site back online.

====================
## CONFIGURATIONS ##
====================

Note: all module settings could be changed only in settings.php file.

1. QUICK CONFIGURATION

To add memcache support for your site you should simply add following settings at the bottom of settings.php:

..
$conf['cache_backends'][] = 'sites/all/modules/memcache_storage/memcache_storage.inc';
$conf['cache_default_class'] = 'MemcacheStorage';
$conf['cache_class_cache_form'] = 'DrupalDatabaseCache';
..

2. ADVANCED CONFIGURATION

Memcache Storage module provides some additional settings for advanced users.

2.1. DEBUG MODE.

Enable debug mode you can by setting 'memcache_storage_debug' to TRUE:

..
$conf['memcache_storage_debug'] = TRUE;
..

Default: FALSE

At the bottom of each page will be displayed all stats about memcache operations.

2.2. KEY PREFIX.

You may add custom prefix to every memcache key.

..
$conf['memcache_storage_key_prefix'] = 'some_key';
..

Default: ''

Note that after changing this setting all cache stored in memcache pool will be flushed.

2.3. COMPRESS MODE.

You may set compress mode for all stored data.

..
$conf['memcache_storage_compress_data'] = 0;
..

Default: 0
Available values: 0 or 2 (MEMCACHE_COMPRESSED)
See: http://www.php.net/manual/en/memcache.set.php

Enabling compress might slightly decrease performance but reduce amount of used by memcache memory.

2.4. COMPRESS THRESHOLD.

Compress Threshold enables automatic compression of large values.

..
$conf['memcache_storage_compress_threshold'] = array(
  'threshold' => 20000,
  'min_savings' => 0.2,
);
..

Default: array('threshold' => 20000, 'min_savings' => 0.2)
See: http://www.php.net/manual/en/memcache.setcompressthreshold.php

2.5. WILDCARDS INVALIDATION.

Memcache is not allowed to flush data using wildcards so we have to store all wildcards and its creation time in variables.
This value allows to flush wildcards after some time when they might be expired to avoid collecting big wildcards arrays.

..
$conf['memcache_storage_wildcard_invalidate'] = 60 * 60 * 24 * 30;  // 30 days.
..

Default: 60 * 60 * 24 * 30 // 30 days.

2.6. MULTIPLE SERVERS SUPPORT.

The available memcached servers are specified in $conf in settings.php. If
you do not specify any servers, memcache.inc assumes that you have a
memcached instance running on localhost:11211. If this is true, and it is
the only memcached instance you wish to use, no further configuration is
required.

If you have more than one memcached instance running, you need to add two
arrays to $conf: memcache_storage_servers and memcache_storage_bins. The arrays follow this
pattern:

..
$conf['memcache_storage_servers'] => array(
  'default'       => 'host1:port',
  'cluster_name2' => 'host2:port',
  'cluster_name3' => 'host3:port',
);

$conf['memcache_storage_bins'] = array(
  'cache'           => 'default',
  'cache_page'      => 'cluster_name2',
  'cache_bootstrap' => 'cluster_name3',
);
..

The bin/cluster/server model can be described as follows:

- Servers are memcached instances identified by host:port.

- Bins are groups of data that get cached together and map 1:1 to the $table
  parameter of cache_set(). Examples from Drupal core are cache_filter,
  cache_menu. The default is 'cache'.

- Clusters are groups of servers that act as a memory pool.

- Many bins can be assigned to a cluster.

- The default cluster is 'default'.

Here is a simple setup that has two memcached instances, both running on
localhost. The 11212 instance belongs to the 'pages' cluster and the table
cache_page is mapped to the 'pages' cluster. Thus everything that gets cached,
with the exception of the page cache (cache_page), will be put into 'default',
or the 11211 instance. The page cache will be in 11212.

..
$conf['memcache_storage_servers'] => array(
  'default' => 'localhost:11211',
  'pages'   => 'localhost:11212',
);

$conf['memcache_storage_bins'] = array(
  'cache'      => 'default',
  'cache_page' => 'pages',
);
..


===========================
## CONFIGARATION EXAMPLE ##
===========================

$conf['cache_backends'][] = 'sites/all/modules/memcache_storage/memcache_storage.inc';
$conf['cache_default_class'] = 'MemcacheStorage';
$conf['cache_class_cache_form'] = 'DrupalDatabaseCache';

$conf['memcache_storage_debug'] = TRUE;
$conf['memcache_storage_compress_data'] = MEMCACHE_COMPRESSED;
$conf['memcache_storage_key_prefix'] = 'build-1';
$conf['memcache_storage_wildcard_invalidate'] = 60 * 60 * 24 * 15; // 15 days.

$conf['memcache_storage_compress_threshold'] = array(
  'threshold' => 10000,
  'min_savings' => 0.5,
);

$conf['memcache_storage_servers'] = array(
  'default'   => '127.0.0.1:11211',
  'bootstrap' => '127.0.0.1:12211',
);

$conf['memcache_storage_bins'] = array(
  'cache' => 'default',
  'cache_bootstrap' => 'bootstrap',
);


=============
## CREDITS ##
=============

Module was designed and developed by Maslouski Yauheni (http://drupalace.ru).
Module development was not sponsored by anyone. It was created for the love of Drupal.
