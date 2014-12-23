<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

/**
 * The Bootstrapper provides an easy to use interface for bootstrapping an application once an environment host is matched.
 *
 * @package Titon\Environment
 */
interface Bootstrapper {

    /**
     * Bootstrap the application by running any final processes or configuration loading for the matched host.
     *
     * @param \Titon\Environment\Host $host
     * @return void
     */
    public function bootstrap(Host $host): void;

}