<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Titon\Common\Base;
use Titon\Common\Mixin\Mutable;
use Titon\Http\Request as BaseRequest;
use Titon\Http\RequestAware;
use Titon\Utility\Config;
use Titon\Utility\Hash;
use Titon\Utility\Time;
use \SessionHandlerInterface;
use \ArrayAccess;
use \IteratorAggregate;
use \Countable;

/**
 * The Session class is a low level manager of session data through the session super global.
 * Provides support for custom handlers, automatic configuration, and more.
 *
 * @package Titon\Http\Server
 */
class Session extends Base implements ArrayAccess, IteratorAggregate, Countable {
    use Mutable, RequestAware;

    /**
     * Configuration.
     *
     * @type array {
     *      @type string $name                  The name of the session cookie
     *      @type array $ini                    Custom ini settings to write
     *      @type bool $checkUserAgent          Validate the user agent hasn't changed between requests
     *      @type bool $checkInactivity         Regenerate the client session if they are idle
     *      @type bool $checkReferrer           Validate the host in the referrer
     *      @type string $inactivityThreshold   The allotted time the client can be inactive
     *      @type string $lifetime              Lifetime of the session cookie
     * }
     */
    protected $_config = [
        'name' => 'titon',
        'ini' => [],
        'checkUserAgent' => true,
        'checkInactivity' => true,
        'checkReferrer' => true,
        'inactivityThreshold' => '+5 minutes',
        'lifetime' => '+10 minutes'
    ];

    /**
     * Storage handler instance.
     *
     * @type \SessionHandlerInterface
     */
    protected $_handler;

    /**
     * The current session id.
     *
     * @type string
     */
    protected $_id;

    /**
     * Set the session and configuration.
     *
     * @param array $config
     */
    public function __construct(array $config = []) {
        $this->_data =& $_SESSION;

        parent::__construct($config + Config::get('session', []));

        register_shutdown_function('session_write_close');
    }

    /**
     * {@inheritdoc}
     */
    public function add(array $params) {
        $_SESSION = array_replace($_SESSION, $params);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function all() {
        return $_SESSION;
    }

    /**
     * Destroy the current session and all values, the session id and remove the session specific cookie.
     *
     * @return $this
     */
    public function destroy() {
        if (!$this->isStarted()) {
            return $this;
        }

        session_destroy();

        $_SESSION = [];
        $this->_id = null;

        // Should this be done in the Response? How to hook up?
        $params = session_get_cookie_params();

        setcookie($this->getConfig('name'), '', time(), $params['path'], $params['domain'], $params['secure'], $params['httponly']);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function get($key, $default = null) {
        $value = Hash::get($_SESSION, $key);

        if ($value === null) {
            return $default;
        }

        return $value;
    }

    /**
     * Return the session handler.
     *
     * @return \SessionHandlerInterface
     */
    public function getHandler() {
        return $this->_handler;
    }

    /**
     * {@inheritdoc}
     */
    public function has($key) {
        return Hash::has($_SESSION, $key);
    }

    /**
     * Returns the current session ID. If no ID is found, regenerate one.
     *
     * @return string
     */
    public function id() {
        if ($this->_id) {
            return $this->_id;

        } else if ($id = session_id()) {
            $this->_id = $id;

            return $id;
        }

        return $this->regenerate();
    }

    /**
     * Return true if the session has started.
     *
     * @return bool
     */
    public function isStarted() {
        return (session_status() === PHP_SESSION_ACTIVE || headers_sent());
    }

    /**
     * {@inheritdoc}
     */
    public function keys() {
        return array_keys($_SESSION);
    }

    /**
     * Return the session name.
     *
     * @return string
     */
    public function name() {
        return session_name();
    }

    /**
     * Persist the session security params.
     *
     * @uses Titon\Utility\Config
     * @uses Titon\Utility\Time
     *
     * @return $this
     */
    public function persist() {
        $this->set('titon', [
            'time' => Time::toUnix($this->getConfig('inactivityThreshold')),
            'host' => $this->getRequest()->getHost(),
            'agent' => md5(Config::salt() . $this->getRequest()->getHeader('User-Agent'))
        ]);

        return $this;
    }

    /**
     * Regenerate the current session and apply a new session ID.
     *
     * @param bool $delete
     * @return string
     */
    public function regenerate($delete = true) {
        session_regenerate_id($delete);

        return $this->_id = session_id();
    }

    /**
     * {@inheritdoc}
     */
    public function remove($key) {
        $_SESSION = Hash::remove($_SESSION, $key);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function set($key, $value = null) {
        $_SESSION = Hash::set($_SESSION, $key, $value);

        return $this;
    }

    /**
     * Set the session handler.
     *
     * @param \SessionHandlerInterface $handler
     * @return $this
     */
    public function setHandler(SessionHandlerInterface $handler) {
        $this->_handler = $handler;

        session_set_save_handler($handler);

        return $this;
    }

    /**
     * Initialize the session and define all applicable INI settings.
     *
     * @uses Titon\Utility\Time
     *
     * @param \Titon\Http\Request $request
     * @return $this
     */
    public function startup(BaseRequest $request) {
        $this->setRequest($request);

        if ($this->isStarted()) {
            return $this->validate();
        }

        ini_set('url_rewriter.tags', '');
        ini_set('session.name', $this->getConfig('name'));
        ini_set('session.use_trans_sid', false);
        ini_set('session.use_cookies', true);
        ini_set('session.use_only_cookies', true);
        ini_set('session.cookie_domain', $request->getHost());

        if ($request->isSecure()) {
            ini_set('session.cookie_secure', true);
        }

        if ($this->getConfig('checkReferrer')) {
            ini_set('session.referer_check', $request->getHost());
        }

        // Lifetime
        $lifetime = $this->getConfig('lifetime');

        if (!is_numeric($lifetime)) {
            $lifetime = Time::toUnix($lifetime) - time();
        }

        ini_set('session.cookie_lifetime', $lifetime);

        // Set custom ini
        if ($ini = $this->getConfig('ini')) {
            foreach ($ini as $key => $value) {
                ini_set($key, $value);
            }
        }

        // Start session
        session_write_close();
        session_name($this->getConfig('name'));
        session_start();

        $this->id();
        $this->validate();

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function toArray() {
        return $_SESSION;
    }

    /**
     * Validate the session and regenerate or destroy if necessary.
     *
     * @uses Titon\Utility\Config
     *
     * @return $this
     */
    public function validate() {
        if ($this->has('titon')) {
            $session = $this->get('titon');

            if ($this->getConfig('checkUserAgent') && $session['agent'] !== md5(Config::salt() . $this->getRequest()->getHeader('User-Agent'))) {
                $this->destroy();
                $this->persist();
            }

            if ($this->getConfig('checkInactivity') && $session['time'] <= time()) {
                $this->regenerate();
                $this->persist();
            }
        } else {
            $this->persist();
        }

        return $this;
    }

}