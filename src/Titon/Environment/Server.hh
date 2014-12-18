<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

/**
 * Enumerable for the types of environment servers.
 *
 * @package Titon\Environment
 */
enum Server: int {
    DEV = 0;
    PROD = 1;
    QA = 2;
    STAGING = 3;
    TESTING = 4;
}