<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment\Event;

use Titon\Environment\Environment;
use Titon\Environment\Host;
use Titon\Event\Event;

/**
 * The environment event that occurs after the environment is detected.
 * The matched host is passed to the event.
 *
 * @package Titon\Environment\Event
 */
class InitializedEvent extends Event {

    /**
     * The environment instance.
     *
     * @var \Titon\Environment\Environment
     */
    protected Environment $env;

    /**
     * The current matched host.
     *
     * @var \Titon\Environment\Host
     */
    protected Host $host;

    /**
     * Store the event settings.
     *
     * @param \Titon\Environment\Environment $env
     * @param \Titon\Environment\Host $host
     */
    public function __construct(Environment $env, Host $host) {
        $this->env = $env;
        $this->host = $host;

        parent::__construct('env.initialized');
    }

    /**
     * Return the environment.
     *
     * @return \Titon\Environment\Environment
     */
    public function getEnvironment(): Environment {
        return $this->env;
    }

    /**
     * Return the matched host.
     *
     * @return \Titon\Environment\Host
     */
    public function getHost(): Host {
        return $this->host;
    }

}
