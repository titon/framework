<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Environment;

use Titon\Utility\Col;

type HostnameList = Vector<string>;

/**
 * A Host represents a shared configuration for a collection of hostnames.
 *
 * @package Titon\Environment
 */
class Host {

    /**
     * List of hostnames.
     *
     * @var \Titon\Environment\HostnameList
     */
    protected HostnameList $_hostnames;

    /**
     * Unique identifier.
     *
     * @var string
     */
    protected string $_key = '';

    /**
     * Type of environment.
     *
     * @var \Titon\Environment\Server
     */
    protected Server $_type;

    /**
     * Set the required settings.
     *
     * @param \Titon\Environment\Server $type
     * @param string|array|Vector $hostnames
     */
    public function __construct(Server $type, mixed $hostnames = Vector {}) {
        $this->_hostnames = Col::toVector($hostnames);
        $this->_type = $type;
    }

    /**
     * Return the list of hostnames.
     *
     * @return \Titon\Environment\HostnameList
     */
    public function getHostnames(): HostnameList {
        return $this->_hostnames;
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
     * @return \Titon\Environment\Server
     */
    public function getType(): Server {
        return $this->_type;
    }

    /**
     * Is the current environment development?
     *
     * @return bool
     */
    public function isDevelopment(): bool {
        return ($this->getType() === Server::DEV);
    }

    /**
     * Is the current environment production?
     *
     * @return bool
     */
    public function isProduction(): bool {
        return ($this->getType() === Server::PROD);
    }

    /**
     * Is the current environment QA?
     *
     * @return bool
     */
    public function isQA(): bool {
        return ($this->getType() === Server::QA);
    }

    /**
     * Is the current environment staging?
     *
     * @return bool
     */
    public function isStaging(): bool {
        return ($this->getType() === Server::STAGING);
    }

    /**
     * Is the current environment testing?
     *
     * @return bool
     */
    public function isTesting(): bool {
        return ($this->getType() === Server::TESTING);
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