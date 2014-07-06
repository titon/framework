<?hh //strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\View\Engine;

use Titon\Common\Base;
use Titon\View\View;
use Titon\View\View\Engine;

/**
 * Defines shared functionality for view engines.
 * Provides support for layouts, wrappers, data and rendering.
 *
 * @package Titon\View\View\Engine
 */
abstract class AbstractEngine extends Base implements Engine {

    /**
     * Configuration.
     *
     * @type array {
     *      @type string $layout    Name of the layout template to wrap content with
     *      @type array $wrapper    List of wrappers to wrap templates with
     * }
     */
    protected $_config = [
        'layout' => 'default',
        'wrapper' => []
    ];

    /**
     * Current parsed template content.
     *
     * @type string
     */
    protected $_content;

    /**
     * Variables currently bound to the engine.
     *
     * @type array
     */
    protected $_variables = [];

    /**
     * View instance.
     *
     * @type \Titon\View\View
     */
    protected $_view;

    /**
     * {@inheritdoc}
     */
    public function data($key, $default = null) {
        return isset($this->_variables[$key]) ? $this->_variables[$key] : $default;
    }

    /**
     * {@inheritdoc}
     */
    public function getContent() {
        return $this->_content;
    }

    /**
     * {@inheritdoc}
     */
    public function getLayout() {
        return $this->getConfig('layout');
    }

    /**
     * {@inheritdoc}
     */
    public function getWrapper() {
        return $this->getConfig('wrapper');
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
    public function open($partial, array $variables = []) {
        return $this->render(
            $this->getView()->locateTemplate($partial, View::PARTIAL),
            $variables + $this->getView()->getVariables()
        );
    }

    /**
     * {@inheritdoc}
     */
    public function setContent($content) {
        $this->_content = (string) $content;

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
     * {@inheritdoc}
     */
    public function useLayout($name) {
        $this->setConfig('layout', (string) $name);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function wrapWith($name) {
        $this->setConfig('wrapper', $name ? (array) $name : []);

        return $this;
    }

}
