<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Titon\Event\Event;
use Titon\Event\ListenerMap;
use Titon\Utility\Sanitize;
use Titon\Utility\Str;
use Titon\View\DataMap;
use Titon\View\Event\RenderedEvent;
use Titon\View\Event\RenderingEvent;
use Titon\View\Exception\MissingHelperException;
use Titon\View\Exception\MissingTagException;
use Titon\View\Helper;
use Titon\View\View;

/**
 * The Helper class acts as the base for all children helpers to extend.
 * Defines methods and properties for HTML tags and attribute generation.
 *
 * @package Titon\View\Helper
 */
abstract class AbstractHelper implements Helper {

    /**
     * Automatic escaping.
     *
     * @var bool
     */
    protected bool $escape = true;

    /**
     * Mapping of HTML tags.
     *
     * @var \Titon\View\Helper\TagMap
     */
    protected TagMap $tags = Map {};

    /**
     * View object.
     *
     * @var \Titon\View\View
     */
    protected ?View $view;

    /**
     * Parses an array of attributes to the HTML equivalent.
     *
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @param Vector<string> $remove
     * @return string
     */
    public function attributes(AttributeMap $attributes, Vector<string> $remove = Vector {}): string {
        $parsed = '';
        $escape = true;

        if ($attributes->contains('escape')) {
            $escape = $attributes['escape'];
            $remove[] = 'escape';
        }

        if ($attributes) {
            ksort($attributes);

            foreach ($attributes as $key => $value) {
                if (in_array($key, $remove)) {
                    continue;
                }

                $value = (string) $value;

                if (($escape instanceof Traversable && !in_array($key, $escape)) || ($escape === true)) {
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
    public function escape(string $value, ?bool $escape = null): string {
        if ($escape === null) {
            $escape = $this->getEscaping();
        }

        if ($escape) {
            $value = Sanitize::escape($value);
        }

        return $value;
    }

    /**
     * Return the automatic escaping setting.
     *
     * @return bool
     */
    public function getEscaping(): bool {
        return $this->escape;
    }

    /**
     * {@inheritdoc}
     */
    public function getHelper(string $key): Helper {
        if ($view = $this->getView()) {
            return $view->getHelper($key);
        }

        throw new MissingHelperException(sprintf('Helper %s does not exist', $key));
    }

    /**
     * Return the HTML for a single tag.
     *
     * @param string $tag
     * @return string
     * @throws \Titon\View\Exception\MissingTagException
     */
    public function getTag(string $tag): string {
        if ($this->getTags()->contains($tag)) {
            return $this->tags[$tag];
        }

        throw new MissingTagException(sprintf('Tag %s does not exist', $tag));
    }

    /**
     * Return all defined HTML tags.
     *
     * @return \Titon\View\Helper\TagMap
     */
    public function getTags(): TagMap {
        return $this->tags;
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
    public function preRender(Event $event): void {
        invariant($event instanceof RenderingEvent, 'Must be a RenderingEvent.');

        $this->setView($event->getView());
    }

    /**
     * {@inheritdoc}
     */
    public function postRender(Event $event): void {
        invariant($event instanceof RenderedEvent, 'Must be a RenderedEvent.');

        $this->setView($event->getView());
    }

    /**
     * {@inheritdoc}
     */
    public function subscribeToEvents(): ListenerMap {
        return Map {
            'view.rendering' => 'preRender',
            'view.rendered' => 'postRender',
        };
    }

    /**
     * Enable or disable automatic escaping.
     *
     * @param bool $escape
     * @return $this
     */
    public function setEscaping(bool $escape): this {
        $this->escape = $escape;

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
     * Generates an HTML tag by interpolating variables into the markup.
     *
     * @param string $tag
     * @param \Titon\View\DataMap $params
     * @return string
     */
    public function tag(string $tag, DataMap $params = Map {}): string {
        return Str::insert($this->getTag($tag), $params, Map {'escape' => false}) . PHP_EOL;
    }

}
