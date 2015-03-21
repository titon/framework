<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

use Titon\Environment\Exception\NoDotEnvConfigException;

/**
 * TODO
 *
 * @package Titon\Environment
 */
class Detector {

    /**
     * The name of the environment variable which holds the type of environment it is.
     *
     * @var string
     */
    protected string $envVarName;

    /**
     * Directory path where the `.env` files are located.
     *
     * @var string
     */
    protected string $lookupPath;

    /**
     * Secure variables loaded on initialization.
     *
     * @var \Titon\Environment\VariableMap
     */
    protected VariableMap $variables = Map {};

    /**
     * Set the variable lookup path and var name.
     *
     * @param string $path
     * @param string $envVarName
     */
    public function __construct(string $path, string $envVarName = 'APP_ENV') {
        $this->lookupPath = rtrim($path, '/') . '/';
        $this->envVarName = $envVarName;
    }

    /**
     * Return the name of the environment.
     *
     * @return string
     */
    public function getEnvironment(): string {
        return $this->getVariable($this->envVarName) ?: getenv($this->envVarName);
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
     * @return string
     */
    public function getVariable(string $key): string {
        return $this->getVariables()->get($key) ?: '';
    }

    /**
     * Return all loaded variables.
     *
     * @return \Titon\Environment\VariableMap
     */
    public function getVariables(): VariableMap {
        return $this->variables;
    }

    /**
     * Detect the environment by locating and loading all `.env` files at the defined lookup path.
     *
     * @throws \Titon\Environment\Exception\NoDotEnvConfigException
     */
    public function initialize(): void {
        $path = $this->getLookupPath();

        // Load the base config
        if (file_exists($path . '.env')) {
            $this->variables = (new Loader($path . '.env'))->getVariables();
        } else {
            throw new NoDotEnvConfigException(sprintf('No .env file exists at lookup path %s', $path));
        }

        // Load environment specific config
        $current = $this->getEnvironment();

        if (file_exists($path . '.env.' . $current)) {
            $this->variables = (new Loader($path . '.env.' . $current, $this->variables))->getVariables();
        }
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
