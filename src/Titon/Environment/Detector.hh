<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

/**
 * A hub that allows you to store different environment host configurations,
 * which can be detected and initialized at runtime.
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
     * Directory path to the secure variables directory.
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
     * Set the `.env` lookup path.
     *
     * @param string $path
     */
    public function __construct(string $path, string $envVarName = 'APP_ENV') {
        $this->lookupPath = Path::ds($path, true);
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
     * Return the secure lookup path.
     *
     * @return string
     */
    public function getLookupPath(): string {
        return $this->lookupPath;
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

    }

    /**
     * Does the current environment match the passed key?
     *
     * @param string $key
     * @return bool
     */
    public function is(string $key): bool {
        return ($this->getEnvironment() === $key);
    }

}
