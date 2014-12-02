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
type XmlNamespaces = Map<string, string>;

/**
 * The XmlElement class represents a single element (or node) with an XML document tree.
 * It has support for attributes, children (nested elements), and an optional value.
 *
 * @package Titon\Type
 */
class XmlElement implements IteratorAggregate<XmlElement>, Countable {

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
     * Map of attributes for document declaration (opening XML tag).
     *
     * @type \Titon\Type\XmlAttributes
     */
    protected XmlAttributes $_declaration = Map {
        'version' => '1.0',
        'encoding' => 'UTF-8'
    };

    /**
     * Name of this element.
     *
     * @type string
     */
    protected string $_name = '';

    /**
     * Map of namespaces for this element.
     *
     * @type \Titon\Type\XmlNamespaces
     */
    protected XmlNamespaces $_namespaces = Map {};

    /**
     * The parent element this child belongs to.
     *
     * @type \Titon\Type\XmlElement
     */
    protected ?XmlElement $_parent = null;

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
     * @param string $namespace
     */
    public function __construct(string $name, XmlAttributes $attributes = Map {}, string $namespace = '') {
        $this->setName($name, $namespace);
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
     * Format attributes for use in an XML document.
     *
     * @param \Titon\Type\XmlAttributes $attributes
     * @return string
     */
    public function formatAttributes(XmlAttributes $attributes): string {
        $xml = '';

        foreach ($attributes as $key => $value) {
            $xml .= sprintf(' %s="%s"', XmlDocument::formatName($key), Sanitize::escape($value));
        }

        return $xml;
    }

    /**
     * Format namespaces as attributes for use in an XML document.
     *
     * @param \Titon\Type\XmlNamespaces $namespaces
     * @return string
     */
    public function formatNamespaces(XmlNamespaces $namespaces): string {
        $xml = '';

        foreach ($namespaces as $key => $value) {
            $xml .= sprintf(' xmlns:%s="%s"', $key, Sanitize::escape($value));
        }

        return $xml;
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
     * Return a type casted value.
     *
     * @return mixed
     */
    public function getBoxedValue(): mixed {
        return XmlDocument::box($this->getValueWithoutCdata());
    }

    /**
     * Returns the first child that matches the defined name.
     *
     * @param string $name
     * @return \Titon\Type\XmlElement
     */
    public function getChild(string $name): ?XmlElement {
        foreach ($this->getChildren() as $child) {
            if ($child->getName() === $name) {
                return $child;
            }
        }

        return null;
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
     * Return a list of children with the defined name.
     *
     * @return \Titon\Type\XmlElementList
     */
    public function getChildrenByName(string $name): XmlElementList {
        return $this->getChildren()->filter( $item ==> $item->getName() === $name );
    }

    /**
     * Return the opening declaration attributes.
     *
     * @return \Titon\Type\XmlAttributes
     */
    public function getDeclaration(): XmlAttributes {
        return $this->_declaration;
    }

    /**
     * Return an iterator.
     *
     * @return Iterator<XmlElement>
     */
    public function getIterator(): Iterator<XmlElement> {
        return $this->getChildren()->getIterator();
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
     * Return a namespace or an empty string if not found.
     *
     * @param string $key
     * @return string
     */
    public function getNamespace(string $key): string {
        return $this->getNamespaces()->get($key) ?: '';
    }

    /**
     * Return all namespaces.
     *
     * @return \Titon\Type\XmlNamespaces
     */
    public function getNamespaces(): XmlNamespaces {
        return $this->_namespaces;
    }

    /**
     * Return all attributes for a specific namespace.
     *
     * @param string $namespace
     * @return \Titon\Type\XmlAttributes
     */
    public function getNamespaceAttributes(string $namespace): XmlAttributes {
        return $this->getAttributes()->filterWithKey( ($key, $value) ==> strpos($key, $namespace . ':') === 0 );
    }

    /**
     * Return all children for a specific namespace.
     *
     * @param string $namespace
     * @return \Titon\Type\XmlElementList
     */
    public function getNamespaceChildren(string $namespace): XmlElementList {
        return $this->getChildren()->filter( $item ==> strpos($item->getName(), $namespace . ':') === 0 );
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
     * Return the value without the wrapping CDATA block.
     *
     * @return string
     */
    public function getValueWithoutCdata(): string {
        $value = $this->getValue();

        if (strpos($value, '<![CDATA[') === 0) {
            $value = mb_substr($value, 10); // Remove opening
            $value = mb_substr($value, 0, mb_strlen($value) - 3); // Remove closing
            $value = trim($value); // Remove newlines
        }

        return $value;
    }

    /**
     * Return true if an attribute exists by name.
     *
     * @param string $key
     * @return bool
     */
    public function hasAttribute(string $key): bool {
        return $this->getAttributes()->contains($key);
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
     * Return true if a namespace exists by name.
     *
     * @param string $key
     * @return bool
     */
    public function hasNamespace(string $key): bool {
        return $this->getNamespaces()->contains($key);
    }

    /**
     * Return true if the element has any namespaces.
     *
     * @return bool
     */
    public function hasNamespaces(): bool {
        return !$this->getNamespaces()->isEmpty();
    }

    /**
     * Return true if the element is a child.
     *
     * @return bool
     */
    public function isChild(): bool {
        return !$this->isRoot();
    }

    /**
     * Return true if the element is the top level parent node.
     *
     * @return bool
     */
    public function isRoot(): bool {
        return ($this->getParent() === null);
    }

    /**
     * Set an attributes value. The value will be type casted and unboxed into a string.
     *
     * @param string $key
     * @param mixed $value
     * @param string $namespace
     * @return $this
     */
    public function setAttribute(string $key, mixed $value, string $namespace = ''): this {
        if ($namespace) {
            $key = $namespace . ':' . $key;
        }

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
     * Set the document type declaration which includes the version and encoding.
     *
     * @param string $version
     * @param string $encoding
     * @param \Titon\Type\XmlAttributes $attributes
     * @return $this
     */
    public function setDeclaration(string $version = '1.0', string $encoding = 'UTF-8', XmlAttributes $attributes = Map {}): this {
        $attributes['version'] = $version;
        $attributes['encoding'] = $encoding;

        $this->_declaration->setAll($attributes);

        return $this;
    }

    /**
     * Set the elements name. Will be inflected to camel case and if the name starts with a number,
     * it will be prefixed with an underscore.
     *
     * @param string $name
     * @param string $namespace
     * @return $this
     */
    public function setName(string $name, string $namespace = ''): this {
        $name = XmlDocument::formatName($name);

        if ($namespace) {
            $name = $namespace . ':' . $name;
        }

        $this->_name = $name;

        return $this;
    }

    /**
     * Set a namespace and its URL.
     *
     * @param string $key
     * @param string $value
     * @return $this
     */
    public function setNamespace(string $key, string $value): this {
        $this->_namespaces[$key] = $value;

        return $this;
    }

    /**
     * Set multiple namespaces.
     *
     * @param \Titon\Type\XmlNamespaces $namespaces
     * @return $this
     */
    public function setNamespaces(XmlNamespaces $namespaces): this {
        foreach ($namespaces as $key => $value) {
            $this->setNamespace($key, $value);
        }

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
     * Return the element as a nested map structure.
     *
     * @return \Titon\Type\XmlMap
     */
    public function toMap(): XmlMap {
        $map = Map {};

        if ($this->hasAttributes()) {
            $map['@attributes'] = $this->getAttributes();
        }

        if ($this->hasChildren()) {

            // UNSAFE
            // Hack gets confused here and won't resolve the Vector
            foreach ($this->getChildren() as $child) {
                $name = $child->getName();

                // An element with that name already exists
                // So we need to turn it into a list
                if ($map->contains($name)) {
                    if (!$map[$name] instanceof Vector) {
                        $map[$name] = new Vector([$map[$name]]);
                    }

                    $list = $map[$name];
                    invariant($list instanceof Vector, 'Element list must be a vector');

                    $map[$name][] = $child->toMap();

                // Set the value directly
                } else {
                    $map[$name] = $child->toMap();
                }
            }

        } else {
            $map['@value'] = $this->getBoxedValue();
        }

        if ($this->isRoot()) {
            return Map {$this->getName() => $map};
        }

        return $map;
    }

    /**
     * Return the element as an XML string. Properly handle namespaces, attributes, children, and values.
     *
     * @param bool $indent
     * @param int $depth
     * @return string
     */
    public function toString(bool $indent = true, int $depth = 0): string {
        $xml = '';

        // Set root XML tag
        if ($this->isRoot()) {
            $xml = sprintf('<?xml%s?>', $this->formatAttributes($this->getDeclaration())) . PHP_EOL;
        }

        // Apply indenting for nested elements
        if ($indent) {
            $xml .= str_repeat('    ', $depth);
        }

        // Build the tag, its attributes and namespaces
        $xml .= sprintf('<%s%s%s',
            $this->getName(),
            $this->formatNamespaces($this->getNamespaces()),
            $this->formatAttributes($this->getAttributes())
        );

        // No children or value so self close
        if (!$this->hasChildren() && !$this->getValue()) {
            return $xml . '/>' . PHP_EOL;
        } else {
            $xml .= '>';
        }

        // Children take precedence over a value
        if ($this->hasChildren()) {
            $xml .= PHP_EOL;

            foreach ($this->getChildren() as $child) {
                $xml .= $child->toString($indent, $depth + 1);
            }

            // Apply indenting for nested elements
            if ($indent) {
                $xml .= str_repeat('    ', $depth);
            }
        } else {
            $xml .= $this->getValue();
        }

        return $xml . sprintf('</%s>', $this->getName()) . PHP_EOL;
    }

}