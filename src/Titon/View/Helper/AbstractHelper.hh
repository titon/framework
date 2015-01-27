<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Titon\Common\DataMap;
use Titon\Event\Event;
use Titon\Event\ListenerMap;
use Titon\Utility\Registry;
use Titon\Utility\Sanitize;
use Titon\Utility\Str;
use Titon\View\Exception\MissingTagException;
use Titon\View\Helper;
use Titon\View\View;

type AttributeMap = Map<string, mixed>;
type TagMap = Map<string, string>;

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
    protected bool $_escape = true;

    /**
     * Mapping of HTML tags.
     *
     * @var \Titon\View\Helper\TagMap
     */
    protected TagMap $_tags = Map {};

    /**
     * View object.
     *
     * @var \Titon\View\View
     */
    protected ?View $_view;

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
        return $this->_escape;
    }

    /**
     * {@inheritdoc}
     */
    public function getHelper(string $name): Helper {
        if ($view = $this->getView()) {
            return $view->getHelper($name);
        }

        $helper = Registry::factory(sprintf('Titon\View\Helper\%sHelper', ucfirst(str_replace('Helper', '', $name))));

        invariant($helper instanceof Helper, 'Must be a Helper');

        return $helper;
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
            return $this->_tags[$tag];
        }

        throw new MissingTagException(sprintf('Tag %s does not exist', $tag));
    }

    /**
     * Return all defined HTML tags.
     *
     * @return \Titon\View\Helper\TagMap
     */
    public function getTags(): TagMap {
        return $this->_tags;
    }

    /**
     * {@inheritdoc}
     */
    public function getView(): ?View {
        return $this->_view;
    }

    /**
     * {@inheritdoc}
     */
    public function preRender(Event $event, View $view, string $template): void {
        $this->setView($view);
    }

    /**
     * {@inheritdoc}
     */
    public function postRender(Event $event, View $view, string $response): void {
        $this->setView($view);
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
        $this->_escape = $escape;

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
     * Generates an HTML tag by interpolating variables into the markup.
     *
     * @param string $tag
     * @param \Titon\Common\DataMap $params
     * @return string
     */
    public function tag(string $tag, DataMap $params = Map {}): string {
        return Str::insert($this->getTag($tag), $params, Map {'escape' => false}) . PHP_EOL;
    }

}
