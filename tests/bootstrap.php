<?php

error_reporting(E_ALL | E_STRICT);

date_default_timezone_set('UTC');

if (!defined('TEST_DIR')) {
    define('TEST_DIR', __DIR__);
}

if (!defined('TEMP_DIR')) {
    define('TEMP_DIR', TEST_DIR . '/tmp');
}

if (!defined('VENDOR_DIR')) {
    define('VENDOR_DIR', dirname(TEST_DIR) . '/vendor');
}

if (!defined('DS')) {
    define('DS', DIRECTORY_SEPARATOR);
}

if (!file_exists(VENDOR_DIR . '/autoload.php')) {
    exit('Please install Composer in the root folder before running tests!');
}

require VENDOR_DIR . '/autoload.php';