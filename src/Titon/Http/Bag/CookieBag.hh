<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Http\Http;
use Titon\Http\RequestAware;
use Titon\Utility\Config;
use Titon\Utility\Converter;
use Titon\Utility\Crypt;
use Titon\Utility\Time;
use Titon\Utility\Col;

/**
 * Bag for interacting with cookies.
 *
 * @package Titon\Http\Bag
 */
class CookieBag extends ParameterBag {
    use RequestAware;

    /**
     * Configuration.
     *
     * @type Map<string, mixed> {
     *      @type string $domain    What domain the cookie should be usable on
     *      @type string $expires   How much time until the cookie expires
     *      @type string $path      Which path should the cookie only be accessible to
     *      @type bool $secure      Should the cookie only be usable across an HTTPS connection
     *      @type bool $httpOnly    Should the cookie only be accessible through PHP and not the Javascript layer
     *      @type string $encrypt   Supply the Crypt cipher that you would like to use for encryption and decryption
     * }
     */
    protected Map<string, mixed> $_config = Map {
        'domain' => '',
        'expires' => '+1 week',
        'path' => '/',
        'secure' => false,
        'httpOnly' => true,
        'encrypt' => Crypt::RIJNDAEL
    };

    /**
     * Set the cookies and configuration.
     *
     * @param array $cookies
     * @param Map<string, mixed> $config
     */
    public function __construct(array $cookies = [], Map<string, mixed> $config = Map {}) {
        $this->applyConfig(Col::merge(Config::get('cookie', Map {}), $config));
        $this->_data = Converter::toMap($cookies);
    }

    /**
     * Unserialize, decode, and decrypt a value when retrieving.
     *
     * @param string $key
     * @param string $value
     * @return mixed
     */
    public function decrypt(string $key, string $value): mixed {
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
    public function encrypt(string $key, mixed $value): string {
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
    public function get(string $key, mixed $default = null): mixed {
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
    public function set(string $key, mixed $value = null): this {
        return parent::set($key, $this->encrypt($key, $value));
    }

    /**
     * Prepare a cookie in the format of an HTTP Set-Cookie header.
     *
     * @param string $key
     * @param mixed $value
     * @param Map<string, mixed> $config
     * @return string
     */
    public function prepare(string $key, mixed $value, Map<string, mixed> $config = Map {}): string {
        $config = Col::merge($this->allConfig(), $config);
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