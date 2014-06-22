<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

use Titon\Environment\Exception\InvalidEnvironmentException;

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
    protected $_bootstrap;

    /**
     * List of host names and IPs.
     *
     * @type string[]
     */
    protected $_hosts;

    /**
     * Unique identifier.
     *
     * @type string
     */
    protected $_key;

    /**
     * Type of environment.
     *
     * @type int
     */
    protected $_type;

    /**
     * Set the required settings.
     *
     * @param string|array $hosts
     * @param string $type
     * @throws \Titon\Environment\Exception\InvalidEnvironmentException
     */
    public function __construct($hosts, $type = Environment::DEV) {
        if (!in_array($type, [Environment::DEV, Environment::PROD, Environment::STAGING, Environment::QA])) {
            throw new InvalidEnvironmentException(sprintf('Invalid %s environment type detected', $type));
        }

        $this->_hosts = (array) $hosts;
        $this->_type = $type;
    }

    /**
     * Return the bootstrap path.
     *
     * @return string
     */
    public function getBootstrap() {
        return $this->_bootstrap;
    }

    /**
     * Return the list of hosts.
     *
     * @return string[]
     */
    public function getHosts() {
        return $this->_hosts;
    }

    /**
     * Return the host key.
     *
     * @return string
     */
    public function getKey() {
        return $this->_key;
    }

    /**
     * Return the environment type.
     *
     * @return string
     */
    public function getType() {
        return $this->_type;
    }

    /**
     * Is the current environment development?
     *
     * @return bool
     */
    public function isDevelopment() {
        return ($this->getType() === Environment::DEVELOPMENT);
    }

    /**
     * Is the current environment production?
     *
     * @return bool
     */
    public function isProduction() {
        return ($this->getType() === Environment::PRODUCTION);
    }

    /**
     * Is the current environment QA?
     *
     * @return bool
     */
    public function isQA() {
        return ($this->getType() === Environment::QA);
    }

    /**
     * Is the current environment staging?
     *
     * @return bool
     */
    public function isStaging() {
        return ($this->getType() === Environment::STAGING);
    }

    /**
     * Set the bootstrap file path.
     *
     * @param string $path
     * @return $this
     */
    public function setBootstrap($path) {
        $this->_bootstrap = $path;

        return $this;
    }

    /**
     * Set the unique key.
     *
     * @param string $key
     * @return $this
     */
    public function setKey($key) {
        $this->_key = $key;

        return $this;
    }

}