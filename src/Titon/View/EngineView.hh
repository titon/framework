<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View;

use Titon\Common\DataMap;
use Titon\View\Engine;
use Titon\View\Engine\TemplateEngine;

/**
 * Adds support for rendering engines which handle the basics of rendering a template.
 *
 * @package Titon\View\View
 * @events
 *      view.rendering.layout|wrapper|template(View $engine, string $template, Template $type)
 *      view.rendered.layout|wrapper|template(View $engine, string $template, Template $type)
 */
class EngineView extends AbstractView {

    /**
     * Template rendering engine.
     *
     * @var \Titon\View\Engine
     */
    protected Engine $engine;

    /**
     * Set the default rendering engine.
     *
     * @param mixed $paths
     * @param string $ext
     */
    public function __construct(mixed $paths, string $ext = 'tpl') {
        parent::__construct($paths, $ext);

        $this->engine = new TemplateEngine();
        $this->engine->setView($this);
    }

    /**
     * Return the rendering engine.
     *
     * @return \Titon\View\Engine
     */
    public function getEngine(): Engine {
        return $this->engine;
    }

    /**
     * {@inheritdoc}
     */
    public function render(string $template, bool $private = false): string {
        return (string) $this->cache([__METHOD__, $template, $private], ($view) ==> {
            $view->emit('view.rendering', [$view, &$template]);

            $engine = $view->getEngine();
            $type = $private ? Template::CLOSED : Template::OPEN;

            // Render template
            $view->renderLoop($template, $type);

            // Apply wrappers
            foreach ($engine->getWrappers() as $wrapper) {
                $view->renderLoop($wrapper, Template::WRAPPER);
            }

            // Apply layout
            if ($layout = $engine->getLayout()) {
                $view->renderLoop($layout, Template::LAYOUT);
            }

            $response = $engine->getContent();

            $view->emit('view.rendered', [$view, &$response]);

            return $response;
        });
    }

    /**
     * Rendering of individual template parts for each type: layout, wrapper, etc.
     *
     * @param string $template
     * @param \Titon\View\Template $type
     * @return $this
     */
    public function renderLoop(string $template, Template $type): this {
        $engine = $this->getEngine();

        if ($type === Template::LAYOUT) {
            $event = 'layout';
        } else if ($type === Template::WRAPPER) {
            $event = 'wrapper';
        } else {
            $event = 'template';
        }

        $this->emit('view.rendering.' . $event, [$this, &$template, $type]);

        $engine->setContent($this->renderTemplate(
            $this->locateTemplate($template, $type),
            $this->getVariables()
        ));

        $this->emit('view.rendered.' . $event, [$this, &$template, $type]);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function renderTemplate(string $path, DataMap $variables = Map {}): string {
        $expires = $variables->get('cache') ?: '+1 day';

        $callback = function() use ($path, $variables) {
            return $this->getEngine()->render($path, $variables);
        };

        if ($storage = $this->getStorage()) {
            return (string) $storage->store(md5($path), $callback, $expires);
        }

        return (string) call_user_func($callback);
    }

    /**
     * Set the rendering engine.
     *
     * @param \Titon\View\Engine $engine
     * @return $this
     */
    public function setEngine(Engine $engine): this {
        $engine->setView($this);

        $this->engine = $engine;

        return $this;
    }

}
