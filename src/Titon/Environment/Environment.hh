<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

use Titon\Environment\Exception\MissingHostException;
use Titon\Environment\Exception\NoHostMatchException;
use Titon\Event\EmitsEvents;
use Titon\Event\Event;
use Titon\Event\Subject;
use Titon\Utility\Col;
use Titon\Utility\Path;
use Titon\Utility\State\Server as ServerGlobal;

/**
 * A hub that allows you to store different environment host configurations,
 * which can be detected and initialized at runtime.
 *
 * @package Titon\Environment
 * @events
 *      env.initializing(Environment $env)
 *      env.initialized(Environment $env, Host $host)
 */
class Environment implements Subject {
    use EmitsEvents;

    /**
     * List of bootstrappers to trigger after a match.
     *
     * @params \Titon\Environment\BootstrapperList
     */
    protected BootstrapperList $bootstrappers = Vector {};

    /**
     * Currently active environment.
     *
     * @var \Titon\Environment\Host
     */
    protected ?Host $current = null;

    /**
     * List of all environments.
     *
     * @var \Titon\Environment\HostMap
     */
    protected HostMap $hosts = Map {};

    /**
     * The fallback environment.
     *
     * @var \Titon\Environment\Host
     */
    protected ?Host $fallback = null;

    /**
     * Directory path to the secure variables directory.
     *
     * @var string
     */
    protected string $securePath = '';

    /**
     * Secure variables loaded on initialization.
     *
     * @var \Titon\Environment\VariableMap
     */
    protected VariableMap $variables = Map {};

    /**
     * Set internal events and the secure variables lookup path.
     *
     * @param string $path
     */
    public function __construct(string $path = '') {
        if ($path) {
            $this->securePath = Path::ds($path, true);
        }

        $this->on('env.initialized', inst_meth($this, 'doLoadSecureVars'), 1);
        $this->on('env.initialized', inst_meth($this, 'doBootstrap'), 2);
    }

    /**
     * Add a bootstrapper instance.
     *
     * @param \Titon\Environment\Bootstrapper $bootstrapper
     * @return $this
     */
    public function addBootstrapper(Bootstrapper $bootstrapper): this {
        $this->bootstrappers[] = $bootstrapper;

        return $this;
    }

    /**
     * Add an environment host to the mapping.
     *
     * @param string $key
     * @param \Titon\Environment\Host $host
     * @return $this
     */
    public function addHost(string $key, Host $host): this {
        $this->hosts[$key] = $host->setKey($key);

        return $this;
    }

    /**
     * Return the current environment.
     *
     * @return \Titon\Environment\Host
     */
    public function current(): ?Host {
        return $this->current;
    }

    /**
     * Loop through all the bootstrappers and trigger the bootstrapping process.
     * This method is automatically called during the `initialized` event.
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\Environment\Environment $env
     * @param \Titon\Environment\Host $host
     * @return mixed
     */
    public function doBootstrap(Event $event, Environment $env, Host $host): mixed {
        foreach ($this->getBootstrappers() as $bootstrapper) {
            $bootstrapper->bootstrap($host);
        }

        return true;
    }

    /**
     * Attempt to load secure variables from the lookup path.
     * This method is automatically called during the `initialized` event.
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\Environment\Environment $env
     * @param \Titon\Environment\Host $host
     * @return mixed
     */
    public function doLoadSecureVars(Event $event, Environment $env, Host $host): mixed {
        $path = $this->securePath;
        $variables = [];

        if (!$path) {
            return true;
        }

        foreach (['.env.php', sprintf('.env.%s.php', $host->getKey())] as $file) {
            if (file_exists($path . $file)) {
                $variables = array_merge($variables, include_file($path . $file));
            }
        }

        $this->variables = Col::toMap($variables);

        return true;
    }

    /**
     * Return the list of bootstrappers.
     *
     * @return \Titon\Environment\BootstrapperList
     */
    public function getBootstrappers(): BootstrapperList {
        return $this->bootstrappers;
    }

    /**
     * Return the fallback environment.
     *
     * @return \Titon\Environment\Host
     */
    public function getFallback(): ?Host {
        return $this->fallback;
    }

    /**
     * Return a host by key.
     *
     * @param string $key
     * @return \Titon\Environment\Host
     * @throws \Titon\Environment\Exception\MissingHostException
     */
    public function getHost(string $key): Host {
        if ($this->hosts->contains($key)) {
            return $this->hosts[$key];
        }

        throw new MissingHostException(sprintf('Environment host %s does not exist', $key));
    }

    /**
     * Returns the list of environments.
     *
     * @return \Titon\Environment\HostMap
     */
    public function getHosts(): HostMap {
        return $this->hosts;
    }

    /**
     * Return the value of a secure variable defined by key.
     *
     * @param string $key
     * @return string
     */
    public function getVariable(string $key): string {
        return $this->getVariables()->get($key) ?: '';
    }

    /**
     * Return all loaded secure variables.
     *
     * @return \Titon\Environment\VariableMap
     */
    public function getVariables(): VariableMap {
        return $this->variables;
    }

    /**
     * Initialize the environment by matching based on server variables or hostnames.
     *
     * @throws \Titon\Environment\Exception\NoHostMatchException
     */
    public function initialize(): void {
        if ($this->getHosts()->isEmpty()) {
            return;
        }

        $this->emit('env.initializing', [$this]);

        // First attempt to match using environment variables
        $current = $this->matchWithVar();

        // If that fails, then attempt to match using hostnames
        $current = $current ?: $this->matchWithHostname();

        // Set the host if found
        if ($current) {
            $this->current = $current;

        // If not found, use the fallback
        } else if ($fallback = $this->getFallback()) {
            $this->current = $current = $fallback;

        // Throw an error if no matches could be found
        } else {
            throw new NoHostMatchException('No environment host matched');
        }

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
        return (bool) $this->current()?->isDevelopment();
    }

    /**
     * Is the current environment production?
     *
     * @return bool
     */
    public function isProduction(): bool {
        return (bool) $this->current()?->isProduction();
    }

    /**
     * Is the current environment QA?
     *
     * @return bool
     */
    public function isQA(): bool {
        return (bool) $this->current()?->isQA();
    }

    /**
     * Is the current environment staging?
     *
     * @return bool
     */
    public function isStaging(): bool {
        return (bool) $this->current()?->isStaging();
    }

    /**
     * Is the current environment testing?
     *
     * @return bool
     */
    public function isTesting(): bool {
        return (bool) $this->current()?->isTesting();
    }

    /**
     * Attempt to match the environment based on hostname.
     *
     * @return \Titon\Environment\Host
     */
    public function matchWithHostname(): ?Host {
        foreach ($this->getHosts() as $host) {
            foreach ($host->getHostnames() as $name) {
                if ($this->isMachine($name)) {
                    return $host;
                }
            }
        }

        return null;
    }

    /**
     * Attempt to match the environment based on a server environment variable.
     *
     * @return \Titon\Environment\Host
     */
    public function matchWithVar(): ?Host {
        try {
            return $this->getHost((string) getenv('APP_ENV'));
        } catch (MissingHostException $e) {
            return null;
        }
    }

    /**
     * Set the fallback environment; fallback must exist before hand.
     *
     * @param string $key
     * @return $this
     */
    public function setFallback(string $key): this {
        $this->fallback = $this->getHost($key);

        return $this;
    }

}
