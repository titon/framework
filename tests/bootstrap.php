<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

// Set timezone for tests
date_default_timezone_set('UTC');

// Set testing constants
define('TEST_DIR', __DIR__);
define('TEMP_DIR', TEST_DIR . '/tmp');
define('VENDOR_DIR', dirname(TEST_DIR) . '/vendor');
define('SRC_DIR', dirname(TEST_DIR) . '/src');
define('DS', DIRECTORY_SEPARATOR);

// Start autoloader
if (!file_exists(VENDOR_DIR . '/autoload.php')) {
    exit('Please install Composer in the root folder before running tests!');
}

require VENDOR_DIR . '/autoload.php';

// Enable the Debugger for testing
Titon\Debug\Debugger::enable();

// Temporary fix until the PHPUnit patch is merged in
// Issue: https://github.com/sebastianbergmann/phpunit/issues/1389
// Patch: https://github.com/sebastianbergmann/phpunit/pull/1391
$suitePath = VENDOR_DIR . '/phpunit/phpunit/src/Framework/TestSuite.php';
$suiteContents = file_get_contents($suitePath);

if (strpos($suiteContents, 'basename(basename') === false) {
    file_put_contents($suitePath, str_replace('basename($filename, \'.php\')', 'basename(basename($filename, \'.hh\'), \'.php\')', $suiteContents));
    clearstatcache(true, $suitePath);
}
