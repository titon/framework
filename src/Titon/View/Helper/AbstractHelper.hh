<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Psr\Http\Message\RequestInterface;
use Titon\Common\DataMap;
use Titon\Event\Event;
use Titon\Event\ListenerMap;
use Titon\Utility\Sanitize;
use Titon\Utility\Str;
use Titon\View\Exception\MissingTagException;
use Titon\View\Helper;
use Titon\View\View;

type AttributeMap = Map<string, mixed>;
type TagMap = Map<string, mixed>;

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
     * @type bool
     */
    protected $_escape = true;

    /**
     * Mapping of HTML tags.
     *
     * @type \Titon\View\Helper\TagMap
     */
    protected TagMap $_tags = Map {};

    /**
     * Request object.
     *
     * @type \Psr\Http\Message\RequestInterface
     */
    protected ?RequestInterface $_request;

    /**
     * View object.
     *
     * @type \Titon\View\View
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

                if (($escape instanceof Traversable && !in_array($key, $escape)) || ($escape === true)) {
                    $value = $this->escape((string) $value, true);
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
            $escape = $this->_escape;
        }

        if ($escape) {
            $value = Sanitize::escape($value);
        }

        return $value;
    }

    /**
     * {@inheritdoc}
     */
    public function getRequest(): ?RequestInterface {
        return $this->_request;
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
    public function preRender(Event $event, View $view, mixed &$template): void {
        $this->setView($view);
    }

    /**
     * {@inheritdoc}
     */
    public function postRender(Event $event, View $view, string &$response): void {
        $this->setView($view);
    }

    /**
     * {@inheritdoc}
     */
    public function registerEvents(): ListenerMap {
        return Map {
            'view.preRender' => 'preRender',
            'view.postRender' => 'postRender',
        };
    }

    /**
     * {@inheritdoc}
     */
    public function setRequest(RequestInterface $request): this {
        $this->_request = $request;

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
     * Generates an HTML tag if it exists.
     *
     * @param string $tag
     * @param \Titon\Common\DataMap $params
     * @return string
     */
    public function tag(string $tag, DataMap $params = Map {}) {
        return Str::insert($this->getTag($tag), $params, Map {'escape' => false}) . PHP_EOL;
    }

}