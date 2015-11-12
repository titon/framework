<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Titon\Common\Exception\InvalidArgumentException;
use Titon\Crypto\Cipher;
use Titon\Utility\Time;

/**
 * The Cookie class represents a standard HTTP cookie.
 *
 * @package Titon\Http
 */
class Cookie {

    /**
     * What domain the cookie should be usable on.
     *
     * @var string
     */
    protected string $domain = '';

    /**
     * The unix timestamp until the cookie expires.
     *
     * @var int
     */
    protected int $expires = 0;

    /**
     * Should the cookie only be accessible through PHP and not the Javascript layer.
     *
     * @var bool
     */
    protected bool $httpOnly = true;

    /**
     * Name of the cookie.
     *
     * @var string
     */
    protected string $name = '';

    /**
     * Which path should the cookie only be accessible to.
     *
     * @var string
     */
    protected string $path = '/';

    /**
     * Should the cookie only be usable across an HTTPS connection.
     *
     * @var bool
     */
    protected bool $secure = false;

    /**
     * The cookie value.
     *
     * @var string
     */
    protected string $value = '';

    /**
     * Setup the cookie.
     *
     * @param string $name
     * @param string $value
     * @param mixed $expires
     * @param string $path
     * @param string $domain
     * @param bool $httpOnly
     * @param bool $secure
     */
    public function __construct(string $name, string $value = '', mixed $expires = 0, string $path = '/', string $domain = '', bool $httpOnly = true, bool $secure = false) {
        $this->setName($name);
        $this->setValue($value);
        $this->setExpires($expires);
        $this->setPath($path);
        $this->setDomain($domain);
        $this->setHttpOnly($httpOnly);
        $this->setSecure($secure);
    }

    /**
     * Return the cookie as a valid HTTP cookie string.
     *
     * @return string
     */
    public function __toString(): string {
        $expires = $this->getExpires();

        if ($expires <= time()) {
            $value = 'deleted';
        } else {
            $value = $this->getValue();
        }

        // Build the cookie
        $cookie  = urlencode($this->getName()) . '=' . $value;
        $cookie .= '; Expires=' . gmdate(Http::DATE_FORMAT, $expires);

        if ($path = $this->getPath()) {
            $cookie .= '; Path=' . $path;
        }

        if ($domain = $this->getDomain()) {
            $cookie .= '; Domain=' . $domain;
        }

        if ($this->isSecure()) {
            $cookie .= '; Secure';
        }

        if ($this->isHttpOnly()) {
            $cookie .= '; HttpOnly';
        }

        return $cookie;
    }

    /**
     * Return the domain.
     *
     * @return string
     */
    public function getDomain(): string {
        return $this->domain;
    }

    /**
     * Return the expires timestamp.
     *
     * @return int
     */
    public function getExpires(): int {
        return $this->expires;
    }

    /**
     * Return the cookie name.
     *
     * @return string
     */
    public function getName(): string {
        return $this->name;
    }

    /**
     * Return the path.
     *
     * @return string
     */
    public function getPath(): string {
        return $this->path;
    }

    /**
     * Return the cookie value. If a `Cipher` is passed, decrypt the value.
     *
     * @param \Titon\Crypto\Cipher $cipher
     * @return string
     */
    public function getValue(?Cipher $cipher = null): string {
        $value = $this->value;

        if ($cipher !== null) {
            $value = (string) $cipher->decrypt($value);
        }

        return $value;
    }

    /**
     * Return true if the cookie disallows JavaScript usage.
     *
     * @return bool
     */
    public function isHttpOnly(): bool {
        return $this->httpOnly;
    }

    /**
     * Return true if HTTPS only.
     *
     * @return bool
     */
    public function isSecure(): bool {
        return $this->secure;
    }

    /**
     * Set the domain.
     *
     * @param string $domain
     * @return $this
     */
    public function setDomain(string $domain): this {
        $this->domain = $domain;

        return $this;
    }

    /**
     * Set the expires timestamp and convert to unix.
     *
     * @param mixed $expires
     * @return $this
     */
    public function setExpires(mixed $expires): this {
        $this->expires = Time::toUnix($expires);

        return $this;
    }

    /**
     * Set the HTTP only flag.
     *
     * @param bool $httpOnly
     * @return $this
     */
    public function setHttpOnly(bool $httpOnly): this {
        $this->httpOnly = $httpOnly;

        return $this;
    }

    /**
     * Set the cookie name.
     *
     * @param string $name
     * @return $this
     */
    public function setName(string $name): this {
        if (preg_match('/[=,;\s\013\014]/', $name)) {
            throw new InvalidArgumentException(sprintf('The cookie name %s contains invalid characters', $name));
        }

        $this->name = $name;

        return $this;
    }

    /**
     * Set the path.
     *
     * @param string $path
     * @return $this
     */
    public function setPath(string $path): this {
        $this->path = $path ?: '/';

        return $this;
    }

    /**
     * Set the secure HTTPS flag.
     *
     * @param bool $secure
     * @return $this
     */
    public function setSecure(bool $secure): this {
        $this->secure = $secure;

        return $this;
    }

    /**
     * Set the cookie value. If a `Cipher` is passed, encrypt the value.
     *
     * @param string $value
     * @param \Titon\Crypto\Cipher $cipher
     * @return $this
     */
    public function setValue(string $value, ?Cipher $cipher = null): this {
        if ($cipher !== null) {
            $value = $cipher->encrypt($value);
        }

        $this->value = $value;

        return $this;
    }

}
