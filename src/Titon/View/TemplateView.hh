<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View;

use Titon\Common\DataMap;
use Titon\Utility\Time;
use Titon\View\Engine;

/**
 * Defines the rendering functionality for basic templating by using a rendering engine.
 * All rendering and templates use basic PHP syntax.
 *
 * @package Titon\View\View
 * @events
 *      view.preTemplate(View $engine, $template, $type), preLayout, preWrapper
 *      view.postTemplate(View $engine, $template, $type), postLayout, postWrapper
 */
class TemplateView extends AbstractView {

    /**
     * Template rendering engine.
     *
     * @type \Titon\View\Engine
     */
    protected Engine $_engine;

    /**
     * Return the rendering engine. Use the default if none was set.
     *
     * @return $this\Engine
     */
    public function getEngine(): Engine {
        if (!$this->_engine) {
            $this->setEngine(new TemplateEngine());
        }

        return $this->_engine;
    }

    /**
     * {@inheritdoc}
     */
    public function render(mixed $template, bool $private = false): string {
        return $this->cache([__METHOD__, $template, $private], function() use ($template, $private) {
            $this->emit('view.preRender', [$this, &$template]);

            $engine = $this->getEngine();
            $type = self::TEMPLATE;

            // Use private templates
            if ($private) {
                $type = self::PRIVATE_TEMPLATE;
            }

            // Render template
            $this->renderLoop($template, $type);

            // Apply wrappers
            foreach ($engine->getWrapper() as $wrapper) {
                $this->renderLoop($wrapper, self::WRAPPER);
            }

            // Apply layout
            if ($layout = $engine->getLayout()) {
                $this->renderLoop($layout, self::LAYOUT);
            }

            $response = $engine->getContent();

            $this->emit('view.postRender', [$this, &$response]);

            return $response;
        });
    }

    /**
     * Render of individual template parts for each type: layout, wrapper, etc.
     *
     * @param string|array $template
     * @param int $type
     * @return $this
     */
    public function renderLoop(mixed $template, int $type): this {
        $engine = $this->getEngine();

        if ($type === self::LAYOUT) {
            $event = 'Layout';
        } else if ($type === self::WRAPPER) {
            $event = 'Wrapper';
        } else {
            $event = 'Template';
        }

        $this->emit('view.pre' . $event, [$this, &$template, $type]);

        $engine->setContent($this->renderTemplate(
            $this->locateTemplate($template, $type),
            $this->getVariables()
        ));

        $this->emit('view.post' . $event, [$this, &$template, $type]);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function renderTemplate(string $path, DataMap $variables = Map {}): string {
        $expires = isset($variables['cache']) ? $variables['cache'] : null;
        $storage = $this->getStorage();
        $key = md5($path);

        if ($expires && $storage) {
            if ($content = $storage->get($key)) {
                return $content;
            }
        }

        $content = $this->getEngine()->render($path, $variables);

        if ($expires && $storage) {
            $storage->set($key, $content, Time::toUnix($expires));
        }

        return $content;
    }

    /**
     * Set the rendering engine.
     *
     * @param \Titon\View\Engine $engine
     * @return $this
     */
    public function setEngine(Engine $engine): this {
        $engine->setView($this);

        $this->_engine = $engine;

        return $this;
    }

}