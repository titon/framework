<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Engine;

use Titon\Common\DataMap;
use Titon\View\Exception\MissingViewException;
use Titon\View\View;
use Titon\View\Engine;
use Titon\View\Engine\WrapperList;
use Titon\View\Template;

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
     * @var string
     */
    protected string $content = '';

    /**
     * Name of the layout template to wrap content with.
     *
     * @var string
     */
    protected string $layout = 'default';

    /**
     * Variables currently bound to the engine.
     *
     * @var \Titon\Common\DataMap
     */
    protected DataMap $variables = Map {};

    /**
     * View instance.
     *
     * @var \Titon\View\View
     */
    protected ?View $view;

    /**
     * List of wrappers to wrap templates with.
     *
     * @var \Titon\View\WrapperList
     */
    protected WrapperList $wrapper = Vector {};

    /**
     * {@inheritdoc}
     */
    public function data(string $key, mixed $default = null): mixed {
        return $this->variables->get($key) ?: $default;
    }

    /**
     * {@inheritdoc}
     */
    public function getContent(): string {
        return $this->content;
    }

    /**
     * {@inheritdoc}
     */
    public function getLayout(): string {
        return $this->layout;
    }

    /**
     * {@inheritdoc}
     */
    public function getWrappers(): WrapperList {
        return $this->wrapper;
    }

    /**
     * {@inheritdoc}
     */
    public function getView(): ?View {
        return $this->view;
    }

    /**
     * {@inheritdoc}
     */
    public function open(string $partial, DataMap $variables = Map {}): string {
        $view = $this->getView();

        if (!$view) {
            throw new MissingViewException('View manager has not been set on this engine');
        }

        return $this->render(
            $view->locateTemplate($partial, Template::PARTIAL),
            $view->getVariables()->toMap()->setAll($variables)
        );
    }

    /**
     * {@inheritdoc}
     */
    public function setContent(string $content): this {
        $this->content = $content;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setView(View $view): this {
        $this->view = $view;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function useLayout(string $name): this {
        $this->layout = $name;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function wrapWith(/* HH_FIXME[4033]: variadic + strict */ ...$names): this {
        $this->wrapper = new Vector($names);

        return $this;
    }

}
