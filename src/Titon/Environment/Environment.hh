<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

use Titon\Common\Configurable;
use Titon\Common\FactoryAware;
use Titon\Environment\Exception\MissingBootstrapException;
use Titon\Environment\Exception\MissingHostException;
use Titon\Event\Emittable;
use Titon\Utility\Path;

/**
 * A hub that allows you to store different environment host configurations,
 * which can be detected and initialized on runtime.
 *
 * @package Titon\Environment
 * @events
 *      env.onInit(Environment $env, Host $host)
 *      env.onBootstrap(Environment $env, Host $host)
 *      env.onFallback(Environment $env, Host $host)
 */
class Environment {
    use Configurable, Emittable, FactoryAware;

    /**
     * Types of environments.
     */
    const string DEV = 'dev';
    const string DEVELOPMENT = 'dev';
    const string STAGING = 'staging';
    const string PROD = 'prod';
    const string PRODUCTION = 'prod';
    const string QA = 'qa';

    /**
     * Default configuration.
     *
     * @type Map<string, mixed>
     */
    protected Map<string, mixed> $_config = Map {
        'bootstrapPath' => '',
        'throwMissingError' => true
    };

    /**
     * Currently active environment.
     *
     * @type \Titon\Environment\Host
     */
    protected ?Host $_current = null;

    /**
     * List of all environments.
     *
     * @type Map<string, \Titon\Environment\Host>
     */
    protected Map<string, Host> $_hosts = Map {};

    /**
     * The fallback environment.
     *
     * @type \Titon\Environment\Host
     */
    protected ?Host $_fallback = null;

    /**
     * Apply configuration.
     *
     * @param Map<string, mixed> $config
     */
    public function __construct(Map<string, mixed> $config = Map {}) {
        $this->applyConfig($config);
    }

    /**
     * Add an environment host and setup the host mapping and fallback.
     *
     * @param string $key
     * @param \Titon\Environment\Host $host
     * @return $this
     */
    public function addHost(string $key, Host $host): this {
        $host->setKey($key);

        // Auto-set bootstrap path
        if ($path = $this->getConfig('bootstrapPath')) {
            $host->setBootstrap(Path::ds($path, true) . $key . '.php');
        }

        $this->_hosts[$key] = $host;

        // Set fallback if empty
        if (!$this->_fallback) {
            $this->setFallback($key);
        }

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
        if (isset($this->_hosts[$key])) {
            return $this->_hosts[$key];
        }

        throw new MissingHostException(sprintf('Environment host %s does not exist', $key));
    }

    /**
     * Returns the list of environments.
     *
     * @return Map<string, \Titon\Environment\Host>
     */
    public function getHosts(): Map<string, Host> {
        return $this->_hosts;
    }

    /**
     * Initialize the environment by including the configuration.
     *
     * @throws \Titon\Environment\Exception\MissingBootstrapException
     */
    public function initialize(): void {
        if ($this->_hosts->isEmpty()) {
            return;
        }

        // Match a host to the machine hostname
        foreach ($this->getHosts() as $host) {
            foreach ($host->getHosts() as $name) {
                if ($this->isMachine($name)) {
                    $this->_current = $host;
                    break 2;
                }
            }
        }

        // If no environment found, use the fallback
        if (!$this->_current) {
            $this->_current = $this->_fallback;
        }

        $current = $this->current();

        $this->emit('env.onInit', [$this, $current]);

        // Bootstrap environment configuration
        if ($bootstrap = $current->getBootstrap()) {
            if (file_exists($bootstrap)) {
                include_once $bootstrap;

                $this->emit('env.onBootstrap', [$this, $current]);

            } else if ($this->getConfig('throwMissingError')) {
                throw new MissingBootstrapException(sprintf('Environment bootstrap for %s does not exist', $current->getKey()));
            }
        }
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
        $name = preg_quote($name, '/');

        // Allow for wildcards
        $name = str_replace('\*', '(.*?)', $name);

        return (bool) preg_match('/^' . $name . '/i', gethostname());
    }

    /**
     * Is the current environment on a localhost?
     *
     * @return bool
     */
    public function isLocalhost(): bool {
        return (in_array($_SERVER['REMOTE_ADDR'], ['127.0.0.1', '::1']) || $_SERVER['HTTP_HOST'] === 'localhost');
    }

    /**
     * Is the current environment development?
     *
     * @return bool
     */
    public function isDevelopment(): bool {
        return $this->current()->isDevelopment();
    }

    /**
     * Is the current environment production?
     *
     * @return bool
     */
    public function isProduction(): bool {
        return $this->current()->isProduction();
    }

    /**
     * Is the current environment QA?
     *
     * @return bool
     */
    public function isQA(): bool {
        return $this->current()->isQA();
    }

    /**
     * Is the current environment staging?
     *
     * @return bool
     */
    public function isStaging(): bool {
        return $this->current()->isStaging();
    }

    /**
     * Set the fallback environment; fallback must exist before hand.
     *
     * @param string $key
     * @return $this
     */
    public function setFallback(string $key): this {
        $this->_fallback = $this->getHost($key);

        $this->emit('env.onFallback', [$this, $this->_fallback]);

        return $this;
    }

}
