<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Titon\Common\Base;
use Titon\Common\Attachable;
use Titon\Event\Event;
use Titon\Http\Request;
use Titon\Utility\Sanitize;
use Titon\Utility\String;
use Titon\View\Helper;
use Titon\View\View;

/**
 * The Helper class acts as the base for all children helpers to extend.
 * Defines methods and properties for HTML tags and attribute generation.
 *
 * @package Titon\View\Helper
 */
abstract class AbstractHelper extends Base implements Helper {
    use Attachable;

    /**
     * Configuration.
     *
     * @type array {
     *      @type bool $escape  Global escaping
     * }
     */
    protected $_config = [
        'escape' => true
    ];

    /**
     * Mapping of HTML tags.
     *
     * @type array
     */
    protected $_tags = [];

    /**
     * Request object.
     *
     * @type \Titon\Http\Request
     */
    protected $_request;

    /**
     * View object.
     *
     * @type \Titon\View\View
     */
    protected $_view;

    /**
     * Parses an array of attributes to the HTML equivalent.
     *
     * @param array $attributes
     * @param array $remove
     * @return string
     */
    public function attributes(array $attributes, array $remove = []) {
        $parsed = '';
        $escape = true;

        if (isset($attributes['escape'])) {
            $escape = $attributes['escape'];
            $remove[] = 'escape';
        }

        if ($attributes) {
            ksort($attributes);

            foreach ($attributes as $key => $value) {
                if (in_array($key, $remove)) {
                    continue;
                }

                if ((is_array($escape) && !in_array($key, $escape)) || ($escape === true)) {
                    $value = $this->escape($value, true);
                }

                $parsed .= ' ' . strtolower($key) . '="' . $value . '"';
            }
        }

        return $parsed;
    }

    /**
     * Escape a value.
     *
     * @param string $value
     * @param bool|null $escape
     * @return string
     */
    public function escape($value, $escape = null) {
        if ($escape === null) {
            $escape = $this->getConfig('escape');
        }

        if ($escape) {
            $value = Sanitize::escape($value);
        }

        return $value;
    }

    /**
     * {@inheritdoc}
     */
    public function getRequest() {
        return $this->_request;
    }

    /**
     * {@inheritdoc}
     */
    public function getView() {
        return $this->_view;
    }

    /**
     * {@inheritdoc}
     */
    public function preRender(Event $event, View $view, &$template) {
        $this->setView($view);
    }

    /**
     * {@inheritdoc}
     */
    public function postRender(Event $event, View $view, &$response) {
        $this->setView($view);
    }

    /**
     * {@inheritdoc}
     */
    public function registerEvents() {
        return [
            'view.preRender' => 'preRender',
            'view.postRender' => 'postRender',
        ];
    }

    /**
     * {@inheritdoc}
     */
    public function setRequest(Request $request) {
        $this->_request = $request;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setView(View $view) {
        $this->_view = $view;

        return $this;
    }

    /**
     * Generates an HTML tag if it exists.
     *
     * @param string $tag
     * @param array $params
     * @return string
     */
    public function tag($tag, array $params = []) {
        return String::insert($this->_tags[$tag], $params, ['escape' => false]) . PHP_EOL;
    }

}