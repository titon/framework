<?hh //strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

use Titon\Environment\Exception\InvalidEnvironmentException;
use Titon\Utility\Converter;

/**
 * A Host represents a shared configuration for a collection of IPs and host names.
 *
 * @package Titon\Environment
 */
class Host {

    /**
     * Path to configuration bootstrap.
     *
     * @type string
     */
    protected string $_bootstrap = '';

    /**
     * List of host names and IPs.
     *
     * @type Vector<string>
     */
    protected Vector<string> $_hosts;

    /**
     * Unique identifier.
     *
     * @type string
     */
    protected string $_key = '';

    /**
     * Type of environment.
     *
     * @type string
     */
    protected string $_type = '';

    /**
     * Set the required settings.
     *
     * @param string|array $hosts
     * @param string $type
     * @throws \Titon\Environment\Exception\InvalidEnvironmentException
     */
    public function __construct(mixed $hosts, string $type = Environment::DEV) {
        if (!in_array($type, [Environment::DEV, Environment::PROD, Environment::STAGING, Environment::QA])) {
            throw new InvalidEnvironmentException(sprintf('Invalid %s environment type detected', $type));
        }

        $this->_hosts = Converter::toVector($hosts);
        $this->_type = $type;
    }

    /**
     * Return the bootstrap path.
     *
     * @return string
     */
    public function getBootstrap(): string {
        return $this->_bootstrap;
    }

    /**
     * Return the list of hosts.
     *
     * @return Vector<string>
     */
    public function getHosts(): Vector<string> {
        return $this->_hosts;
    }

    /**
     * Return the host key.
     *
     * @return string
     */
    public function getKey(): string {
        return $this->_key;
    }

    /**
     * Return the environment type.
     *
     * @return string
     */
    public function getType(): string {
        return $this->_type;
    }

    /**
     * Is the current environment development?
     *
     * @return bool
     */
    public function isDevelopment(): bool {
        return ($this->getType() === Environment::DEVELOPMENT);
    }

    /**
     * Is the current environment production?
     *
     * @return bool
     */
    public function isProduction(): bool {
        return ($this->getType() === Environment::PRODUCTION);
    }

    /**
     * Is the current environment QA?
     *
     * @return bool
     */
    public function isQA(): bool {
        return ($this->getType() === Environment::QA);
    }

    /**
     * Is the current environment staging?
     *
     * @return bool
     */
    public function isStaging(): bool {
        return ($this->getType() === Environment::STAGING);
    }

    /**
     * Set the bootstrap file path.
     *
     * @param string $path
     * @return $this
     */
    public function setBootstrap(string $path): this {
        $this->_bootstrap = $path;

        return $this;
    }

    /**
     * Set the unique key.
     *
     * @param string $key
     * @return $this
     */
    public function setKey(string $key): this {
        $this->_key = $key;

        return $this;
    }

}