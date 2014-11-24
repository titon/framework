<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

use Titon\Common\FactoryAware;
use Titon\Environment\Exception\MissingHostException;
use Titon\Environment\Exception\NoHostMatchException;
use Titon\Event\Emittable;
use Titon\Event\Subject;
use Titon\Utility\Path;
use Titon\Utility\State\Server as ServerGlobal;

type HostMap = Map<string, Host>;

enum Server: string {
    DEV = 'dev';
    PROD = 'prod';
    QA = 'qa';
    STAGING = 'staging';
    TESTING = 'testing';
}

/**
 * A hub that allows you to store different environment host configurations,
 * which can be detected and initialized on runtime.
 *
 * @package Titon\Environment
 * @events
 *      env.initializing(Environment $env)
 *      env.initialized(Environment $env, Host $host)
 */
class Environment implements Subject {
    use Emittable, FactoryAware;

    /**
     * Root path to the bootstrap directory.
     *
     * @params string
     */
    protected string $_bootstrapPath = '';

    /**
     * Currently active environment.
     *
     * @type \Titon\Environment\Host
     */
    protected ?Host $_current = null;

    /**
     * List of all environments.
     *
     * @type \Titon\Environment\HostMap
     */
    protected HostMap $_hosts = Map {};

    /**
     * The fallback environment.
     *
     * @type \Titon\Environment\Host
     */
    protected ?Host $_fallback = null;

    /**
     * Set the bootstrap directory path.
     *
     * @param string $path
     */
    public function __construct(string $path = '') {
        $this->setBootstrapPath($path);
    }

    /**
     * Add an environment host to the mapping.
     * Automatically define a bootstrap file if the path has been set.
     *
     * @param string $key
     * @param \Titon\Environment\Host $host
     * @return $this
     */
    public function addHost(string $key, Host $host): this {
        $host->setKey($key);

        // Auto-set bootstrap path
        if ($path = $this->getBootstrapPath()) {
            $host->setBootstrap(Path::ds($path, true) . $key . '.php');
        }

        $this->_hosts[$key] = $host;

        return $this;
    }

    /**
     * Return the current environment.
     *
     * @return \Titon\Environment\Host
     */
    public function current(): ?Host {
        return $this->_current;
    }

    /**
     * Return the bootstrap path.
     *
     * @return string
     */
    public function getBootstrapPath(): string {
        return $this->_bootstrapPath;
    }

    /**
     * Return the fallback environment.
     *
     * @return \Titon\Environment\Host
     */
    public function getFallback(): ?Host {
        return $this->_fallback;
    }

    /**
     * Return a host by key.
     *
     * @param string $key
     * @return \Titon\Environment\Host
     * @throws \Titon\Environment\Exception\MissingHostException
     */
    public function getHost(string $key): Host {
        if ($this->_hosts->contains($key)) {
            return $this->_hosts[$key];
        }

        throw new MissingHostException(sprintf('Environment host %s does not exist', $key));
    }

    /**
     * Returns the list of environments.
     *
     * @return \Titon\Environment\HostMap
     */
    public function getHosts(): HostMap {
        return $this->_hosts;
    }

    /**
     * Initialize the environment by including the configuration.
     *
     * @param bool $throwError
     */
    public function initialize(bool $throwError = false): void {
        $hosts = $this->getHosts();

        if ($hosts->isEmpty()) {
            return;
        }

        $this->emit('env.initializing', [$this]);

        // Match a host to the machine hostname
        $current = null;

        foreach ($hosts as $host) {
            if ($current !== null) {
                break;
            }

            foreach ($host->getHostnames() as $name) {
                if ($this->isMachine($name)) {
                    $current = $host;
                    break;
                }
            }
        }

        if ($current) {
            $this->_current = $current;

        // If no environment found, use the fallback
        } else if ($fallback = $this->getFallback()) {
            $this->_current = $current = $fallback;

        // Throw an error if no fallback defined and no match
        } else {
            throw new NoHostMatchException('No host matched for environment bootstrapping');
        }

        // Bootstrap environment configuration
        $current->bootstrap($throwError);

        $this->emit('env.initialized', [$this, $current]);
    }

    /**
     * Does the current environment match the passed key?
     *
     * @param string $key
     * @return bool
     */
    public function is(string $key): bool {
        return ($this->current() === $this->getHost($key));
    }

    /**
     * Determine if the name matches the host machine name.
     *
     * @param string $name
     * @return bool
     */
    public function isMachine(string $name): bool {
        $host = gethostname();

        if ($name === $host) {
            return true;
        }

        // Allow for wildcards
        return (bool) preg_match('/^' . str_replace('\*', '(.*?)', preg_quote($name, '/')) . '/i', gethostname());
    }

    /**
     * Is the current environment on a localhost?
     *
     * @return bool
     */
    public function isLocalhost(): bool {
        return (in_array(ServerGlobal::get('REMOTE_ADDR'), ['127.0.0.1', '::1']) || ServerGlobal::get('HTTP_HOST') === 'localhost');
    }

    /**
     * Is the current environment development?
     *
     * @return bool
     */
    public function isDevelopment(): bool {
        return (($current = $this->current()) && $current->isDevelopment());
    }

    /**
     * Is the current environment production?
     *
     * @return bool
     */
    public function isProduction(): bool {
        return (($current = $this->current()) && $current->isProduction());
    }

    /**
     * Is the current environment QA?
     *
     * @return bool
     */
    public function isQA(): bool {
        return (($current = $this->current()) && $current->isQA());
    }

    /**
     * Is the current environment staging?
     *
     * @return bool
     */
    public function isStaging(): bool {
        return (($current = $this->current()) && $current->isStaging());
    }

    /**
     * Is the current environment testing?
     *
     * @return bool
     */
    public function isTesting(): bool {
        return (($current = $this->current()) && $current->isTesting());
    }

    /**
     * Set the bootstrap path directory.
     *
     * @param string $path
     * @return $this
     */
    public function setBootstrapPath(string $path): this {
        $this->_bootstrapPath = $path;

        return $this;
    }

    /**
     * Set the fallback environment; fallback must exist before hand.
     *
     * @param string $key
     * @return $this
     */
    public function setFallback(string $key): this {
        $this->_fallback = $this->getHost($key);

        return $this;
    }

}
