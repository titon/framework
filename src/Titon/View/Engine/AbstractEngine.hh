<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Engine;

use Titon\Common\DataMap;
use Titon\View\View;
use Titon\View\Engine;
use Titon\View\Template;
use Titon\View\WrapperList;

/**
 * Defines shared functionality for view engines.
 * Provides support for layouts, wrappers, data and rendering.
 *
 * @package Titon\View\Engine
 */
abstract class AbstractEngine implements Engine {

    /**
     * Current parsed template.
     *
     * @type string
     */
    protected string $_content = '';

    /**
     * Name of the layout template to wrap content with.
     *
     * @type string
     */
    protected string $_layout = 'default';

    /**
     * Variables currently bound to the engine.
     *
     * @type \Titon\Common\DataMap
     */
    protected DataMap $_variables = Map {};

    /**
     * View instance.
     *
     * @type \Titon\View\View
     */
    protected View $_view;

    /**
     * List of wrappers to wrap templates with.
     *
     * @type \Titon\View\WrapperList
     */
    protected WrapperList $_wrapper = Vector {};

    /**
     * {@inheritdoc}
     */
    public function data(string $key, mixed $default = null): mixed {
        return $this->_variables->get($key) ?: $default;
    }

    /**
     * {@inheritdoc}
     */
    public function getContent(): string {
        return $this->_content;
    }

    /**
     * {@inheritdoc}
     */
    public function getLayout(): string {
        return $this->_layout;
    }

    /**
     * {@inheritdoc}
     */
    public function getWrappers(): WrapperList {
        return $this->_wrapper;
    }

    /**
     * {@inheritdoc}
     */
    public function getView(): View {
        return $this->_view;
    }

    /**
     * {@inheritdoc}
     */
    public function open(string $partial, DataMap $variables = Map {}): string {
        return $this->render(
            $this->getView()->locateTemplate($partial, Template::PARTIAL),
            $this->getView()->getVariables()->toMap()->setAll($variables)
        );
    }

    /**
     * {@inheritdoc}
     */
    public function setContent(string $content): this {
        $this->_content = $content;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setView(View $view): this {
        $this->_view = $view;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function useLayout(string $name): this {
        $this->_layout = $name;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function wrapWith(...$names): this {
        $this->_wrapper = new Vector($names);

        return $this;
    }

}
