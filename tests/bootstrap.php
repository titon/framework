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

$loader = require VENDOR_DIR . '/autoload.php';
$loader->add('', TEST_DIR);

// Temporary fix until the PHPUnit patch is merged in
// Issue: https://github.com/sebastianbergmann/phpunit/issues/1389
// Patch: https://github.com/sebastianbergmann/phpunit/pull/1391
$suitePath = __DIR__ . '/../vendor/phpunit/phpunit/src/Framework/TestSuite.php';
$suiteContents = file_get_contents($suitePath);

if (strpos($suiteContents, 'basename(basename') === false) {
    file_put_contents($suitePath, str_replace('basename($filename, \'.php\')', 'basename(basename($filename, \'.hh\'), \'.php\')', $suiteContents));
    clearstatcache(true, $suitePath);
}

// Temporary fix until the PHPUnit patch is merged in
// Issue: https://github.com/sebastianbergmann/phpunit/issues/1308
// Patch: https://github.com/sebastianbergmann/phpunit/commit/42f3bd57dc5892d707ff0c4027e66f3424ae5b00
$statePath = __DIR__ . '/../vendor/phpunit/phpunit/src/Util/GlobalState.php';
$stateContents = file_get_contents($statePath);

if (strpos($suiteContents, 'vfs') === false) {
    $replacement = <<<'CODE'
if (preg_match('/^(vfs|phpvfs[a-z0-9]+):/', $file)) {
                continue;
            }

            if (!$blacklist->isBlacklisted($file) && is_file($file)) {
CODE;

    file_put_contents($statePath, str_replace('if (!$blacklist->isBlacklisted($file) && is_file($file)) {', $replacement, $stateContents));
    clearstatcache(true, $statePath);
}