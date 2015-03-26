<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

use Titon\Environment\Exception\NoDotEnvConfigException;

/**
 * The Detector provides a direct hub for loading and parsing `.env` files for variables,
 * as well as simple environment detection by name.
 *
 * @package Titon\Environment
 */
class Detector {

    /**
     * Directory path where the `.env` files are located.
     *
     * @var string
     */
    protected string $lookupPath;

    /**
     * Secure variables loaded on initialization.
     *
     * @var \Titon\Environment\ImmutableVariableMap
     */
    protected ImmutableVariableMap $variables = ImmMap {};

    /**
     * Set the variable lookup path and var name.
     *
     * @param string $path
     */
    public function __construct(string $path) {
        $this->lookupPath = rtrim($path, '/') . '/';
    }

    /**
     * Return the name of the environment.
     *
     * @return string
     */
    public function getEnvironment(): string {
        return $this->getVariable('APP_ENV') ?: getenv('APP_ENV') ?: 'prod';
    }

    /**
     * Return the variable lookup path.
     *
     * @return string
     */
    public function getLookupPath(): string {
        return $this->lookupPath;
    }

    /**
     * Return the value of a variable defined by key.
     *
     * @param string $key
     * @return mixed
     */
    public function getVariable(string $key): mixed {
        return $this->getVariables()->get($key) ?: '';
    }

    /**
     * Return all loaded variables.
     *
     * @return \Titon\Environment\ImmutableVariableMap
     */
    public function getVariables(): ImmutableVariableMap {
        return $this->variables;
    }

    /**
     * Detect the environment by locating and loading all `.env` files at the defined lookup path.
     *
     * @throws \Titon\Environment\Exception\NoDotEnvConfigException
     */
    public function initialize(): void {
        $path = $this->getLookupPath();
        $envPath = $path . '.env';
        $variables = Map {};

        // Load the base config
        if (file_exists($envPath) && is_readable($envPath)) {
            $variables = (new Loader($envPath))->getVariables();
        } else {
            throw new NoDotEnvConfigException(sprintf('Environment file %s does not exist or is not readable', $envPath));
        }

        // Load environment specific config
        $current = $variables->get('APP_ENV') ?: getenv('APP_ENV');

        if ($current && file_exists($path . '.env.' . $current)) {
            $variables = (new Loader($path . '.env.' . $current, $variables))->getVariables();
        }

        $this->variables = $variables->toImmMap();
    }

    /**
     * Does the current environment match?
     *
     * @param string $type
     * @return bool
     */
    public function is(string $type): bool {
        return ($this->getEnvironment() === $type);
    }

}
