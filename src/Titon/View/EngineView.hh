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
use Titon\View\Event\RenderedEvent;
use Titon\View\Event\RenderedTemplateEvent;
use Titon\View\Event\RenderingEvent;
use Titon\View\Event\RenderingTemplateEvent;

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
            $engine = $view->getEngine();

            // Emit before event
            $event = new RenderingEvent($view, $template);
            $view->emit($event);
            $template = $event->getTemplate();

            // Render template
            $view->renderLoop($template, $private ? Template::CLOSED : Template::OPEN);

            // Apply wrappers
            foreach ($engine->getWrappers() as $wrapper) {
                $view->renderLoop($wrapper, Template::WRAPPER);
            }

            // Apply layout
            if ($layout = $engine->getLayout()) {
                $view->renderLoop($layout, Template::LAYOUT);
            }

            // Emit after event
            $event = new RenderedEvent($view, $engine->getContent());
            $view->emit($event);

            return $event->getContent();
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

        // Emit before event
        $event = new RenderingTemplateEvent($this, $template, $type);
        $this->emit($event);
        $template = $event->getTemplate();

        // Render content
        $content = $this->renderTemplate(
            $this->locateTemplate($template, $type),
            $this->getVariables()
        );

        // Emit after event
        $event = new RenderedTemplateEvent($this, $content, $type);
        $this->emit($event);
        $content = $event->getContent();

        // Set content
        $engine->setContent($content);

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
