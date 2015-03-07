<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment\Event;

use Titon\Environment\Environment;
use Titon\Event\Event;

/**
 * The environment event that occurs before the environment is detected.
 *
 * @package Titon\Environment\Event
 */
class InitializingEvent extends Event {

    /**
     * The environment instance.
     *
     * @var \Titon\Environment\Environment
     */
    protected Environment $env;

    /**
     * Store the event settings.
     *
     * @param \Titon\Environment\Environment $env
     */
    public function __construct(Environment $env) {
        $this->env = $env;

        parent::__construct('env.initializing');
    }

    /**
     * Return the environment.
     *
     * @return \Titon\Environment\Environment
     */
    public function getEnvironment(): Environment {
        return $this->env;
    }

}
