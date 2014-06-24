<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Common\Bag\AbstractBag;
use Titon\Common\Config;
use Titon\Http\Http;
use Titon\Http\RequestAware;
use Titon\Utility\Crypt;
use Titon\Utility\Time;

/**
 * Bag for interacting with cookies.
 *
 * @package Titon\Http\Bag
 */
class CookieBag extends AbstractBag {
    use RequestAware;

    /**
     * Configuration.
     *
     * @type array {
     *      @type string $domain    What domain the cookie should be usable on
     *      @type string $expires   How much time until the cookie expires
     *      @type string $path      Which path should the cookie only be accessible to
     *      @type bool $secure      Should the cookie only be usable across an HTTPS connection
     *      @type bool $httpOnly    Should the cookie only be accessible through PHP and not the Javascript layer
     *      @type string $encrypt   Supply the Crypt cipher that you would like to use for encryption and decryption
     * }
     */
    protected $_config = [
        'domain' => '',
        'expires' => '+1 week',
        'path' => '/',
        'secure' => false,
        'httpOnly' => true,
        'encrypt' => Crypt::RIJNDAEL
    ];

    /**
     * Set the cookies and configuration.
     *
     * @param array $cookies
     * @param array $config
     */
    public function __construct(array $cookies = [], array $config = []) {
        $this->applyConfig($config + Config::get('cookie', []));
        $this->_data = $cookies;
    }

    /**
     * Unserialize, decode, and decrypt a value when retrieving.
     *
     * @param string $key
     * @param string $value
     * @return mixed
     */
    public function decrypt($key, $value) {
        if ($cipher = $this->getConfig('encrypt')) {
            $value = Crypt::decrypt($value, $key, $cipher);
        }

        return unserialize(base64_decode($value));
    }

    /**
     * Serialize, encode, and encrypt a value before setting.
     *
     * @param string $key
     * @param mixed $value
     * @return string
     */
    public function encrypt($key, $value) {
        $value = base64_encode(serialize($value));

        if ($cipher = $this->getConfig('encrypt')) {
            $value = Crypt::encrypt($value, $key, $cipher);
        }

        return $value;
    }

    /**
     * Return a cookie defined by key and decrypt the value.
     *
     * @param string $key
     * @param mixed $default
     * @return mixed
     */
    public function get($key, $default = null) {
        if ($value = parent::get($key)) {
            return $this->decrypt($key, $value);
        }

        return $default;
    }

    /**
     * Set a cookie and apply optional configuration.
     * Encrypt the value before setting as a value.
     *
     * @param string $key
     * @param mixed $value
     * @return $this
     */
    public function set($key, $value = null) {
        return parent::set($key, $this->encrypt($key, $value));
    }

    /**
     * Prepare a cookie in the format of an HTTP Set-Cookie header.
     *
     * @param string $key
     * @param mixed $value
     * @param array $config
     * @return string
     */
    public function prepare($key, $value, array $config = []) {
        $config = $config + $this->allConfig();
        $expires = Time::toUnix($config['expires']);

        // Determine the value
        if ($expires <= time()) {
            $value = 'deleted';
        } else {
            $value = $this->encrypt($key, $value);
        }

        // Build the cookie
        $cookie  = urlencode($key) . '=' . urlencode($value);
        $cookie .= '; Expires=' . gmdate(Http::DATE_FORMAT, $expires);

        if ($path = $config['path']) {
            $cookie .= '; Path=' . $path;
        }

        if ($domain = $config['domain']) {
            $cookie .= '; Domain=' . $domain;
        }

        if ($config['secure']) {
            $cookie .= '; Secure';
        }

        if ($config['httpOnly']) {
            $cookie .= '; HttpOnly';
        }

        return $cookie;
    }

}