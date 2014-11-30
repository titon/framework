<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

use Titon\Utility\Sanitize;
use \IteratorAggregate;
use \Countable;

type XmlAttributes = Map<string, string>;
type XmlElementList = Vector<XmlElement>;

/**
 * The XmlElement class represents a single element (or node) with an XML document tree.
 * It has support for attributes, children (nested elements), and an optional value.
 *
 * @package Titon\Type
 */
class XmlElement implements Countable {

    /**
     * Map of attributes for this element.
     *
     * @type \Titon\Type\XmlAttributes
     */
    protected XmlAttributes $_attributes = Map {};

    /**
     * List of children within this element.
     *
     * @type \Titon\Type\XmlElementList
     */
    protected XmlElementList $_children = Vector {};

    /**
     * Name of this element.
     *
     * @type string
     */
    protected string $_name;

    /**
     * The parent element this child belongs to.
     *
     * @type \Titon\Type\XmlElement
     */
    protected XmlElement $_parent = null;

    /**
     * The value within the element. Is override by children.
     *
     * @type string
     */
    protected string $_value = '';

    /**
     * Create a new element and optionally set attributes.
     *
     * @param string $name
     * @param \Titon\Type\XmlAttributes $attributes
     */
    public function __construct(string $name, XmlAttributes $attributes = Map {}) {
        $this->_name = XmlDocument::formatName($name);
        $this->setAttributes($attributes);
    }

    /**
     * Return the class as an XML string.
     *
     * @return string
     */
    public function __toString(): string {
        return $this->toString();
    }

    /**
     * Add a child to this element.
     *
     * @param \Titon\Type\XmlElement $child
     * @return $this
     */
    public function addChild(XmlElement $child): this {
        $child->setParent($this);

        $this->_children[] = $child;

        return $this;
    }

    /**
     * Add multiple children to this element.
     *
     * @param \Titon\Type\XmlElementList $children
     * @return $this
     */
    public function addChildren(XmlElementList $children): this {
        foreach ($children as $child) {
            $this->addChild($child);
        }

        return $this;
    }

    /**
     * Return a count of how many children.
     *
     * @return int
     */
    public function count(): int {
        return count($this->_children);
    }

    /**
     * Return an attributes value or an empty string if not found.
     *
     * @param string $key
     * @return string
     */
    public function getAttribute(string $key): string {
        return $this->getAttributes()->get($key) ?: '';
    }

    /**
     * Return all attributes.
     *
     * @return \Titon\Type\XmlAttributes
     */
    public function getAttributes(): XmlAttributes {
        return $this->_attributes;
    }

    /**
     * Return a list of children.
     *
     * @return \Titon\Type\XmlElementList
     */
    public function getChildren(): XmlElementList {
        return $this->_children;
    }

    /**
     * Return the name of the element.
     *
     * @return string
     */
    public function getName(): string {
        return $this->_name;
    }

    /**
     * Return the parent element or null if it does not exist.
     *
     * @return \Titon\Type\XmlElement
     */
    public function getParent(): ?XmlElement {
        return $this->_parent;
    }

    /**
     * Return the value. If CDATA was set, then the value will be wrapped in CDATA tags.
     *
     * @return string
     */
    public function getValue(): string {
        return $this->_value;
    }

    /**
     * Return true if the element has any attributes.
     *
     * @return bool
     */
    public function hasAttributes(): bool {
        return !$this->getAttributes()->isEmpty();
    }

    /**
     * Return true if the element has children.
     *
     * @return bool
     */
    public function hasChildren(): bool {
        return !$this->getChildren()->isEmpty();
    }

    /**
     * Return true if the element is the top level parent node.
     *
     * @return bool
     */
    public function isRoot(): bool {
        return !$this->getParent();
    }

    /**
     * Set an attributes value. The value will be type casted and unboxed into a string.
     *
     * @param string $key
     * @param mixed $value
     * @return $this
     */
    public function setAttribute(string $key, mixed $value): this {
        $this->_attributes[$key] = XmlDocument::unbox($value);

        return $this;
    }

    /**
     * Set multiple attributes.
     *
     * @param \Titon\Type\XmlAttributes $attributes
     * @return $this
     */
    public function setAttributes(XmlAttributes $attributes): this {
        foreach ($attributes as $key => $value) {
            $this->setAttribute($key, $value);
        }

        return $this;
    }

    /**
     * Set the elements name. Will be inflected to camel case and if the name starts with a number,
     * it will be prefixed with an underscore.
     *
     * @param string $name
     * @return $this
     */
    public function setName(string $name): this {
        $this->_name = XmlDocument::formatName($name);

        return $this;
    }

    /**
     * Set the parent element.
     *
     * @param \Titon\Type\XmlElement $element
     * @return $this
     */
    public function setParent(XmlElement $element): this {
        $this->_parent = $element;

        return $this;
    }

    /**
     * Set the value. If `$cdata` is true, then wrap in a CDATA block.
     *
     * @param mixed $value
     * @param bool $cdata
     * @return $this
     */
    public function setValue(mixed $value, bool $cdata = false): this {
        $value = XmlDocument::unbox($value);

        if ($cdata) {
            $value = '<![CDATA[' . PHP_EOL . $value . PHP_EOL . ']]>';
        }

        $this->_value = $value;

        return $this;
    }

    /**
     * Return the element in a nested map structure.
     *
     * @return\Titon\Type\XmlMap
     */
    public function toMap(): XmlMap {
        return Map {}; // tood
    }

    /**
     * Return the element as an XML string. Properly handle attributes, children, and values.
     *
     * @param bool $indent
     * @param int $depth
     * @return string
     */
    public function toString(bool $indent = true, int $depth = 0): string {
        $xml = '<' . $this->getName();

        foreach ($this->getAttributes() as $key => $value) {
            $xml .= sprintf(' %s="%s"', XmlDocument::formatName($key), Sanitize::escape($value));
        }

        // No children or value so self close
        if (!$this->hasChildren() && !$this->getValue()) {
            return $xml . '/>' . PHP_EOL;
        }

        $xml .= '>';

        // Children take precedence over a value
        if ($this->hasChildren()) {
            $xml .= PHP_EOL;

            foreach ($this->getChildren() as $child) {
                if ($indent) {
                    $xml .= str_repeat('    ', $depth);
                }

                $xml .= $child->toString($indent, $depth + 1);
            }
        } else {
            $xml .= $this->getValue();
        }

        return $xml . sprintf('</%s>', $this->getName()) . PHP_EOL;
    }

}