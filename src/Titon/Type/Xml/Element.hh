<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type\Xml;

use Titon\Type\Xml;
use Titon\Utility\Sanitize;
use \IteratorAggregate;
use \Countable;

/**
 * The Element class represents a single element (or node) with an XML document tree.
 * It has support for attributes, children (nested elements), and an optional value.
 *
 * @package Titon\Type
 */
class Element implements IteratorAggregate<Element>, Countable {

    /**
     * Map of attributes for this element.
     *
     * @var \Titon\Type\Xml\AttributeMap
     */
    protected AttributeMap $attributes = Map {};

    /**
     * List of children within this element.
     *
     * @var \Titon\Type\Xml\ElementList
     */
    protected ElementList $children = Vector {};

    /**
     * Map of attributes for document declaration (opening XML tag).
     *
     * @var \Titon\Type\Xml\AttributeMap
     */
    protected AttributeMap $declaration = Map {
        'version' => '1.0',
        'encoding' => 'UTF-8'
    };

    /**
     * Name of this element.
     *
     * @var string
     */
    protected string $name = '';

    /**
     * Map of namespaces for this element.
     *
     * @var \Titon\Type\Xml\NamespaceMap
     */
    protected NamespaceMap $namespaces = Map {};

    /**
     * The parent element this child belongs to.
     *
     * @var \Titon\Type\Xml\Element
     */
    protected ?Element $parent = null;

    /**
     * The value within the element. Is override by children.
     *
     * @var string
     */
    protected string $value = '';

    /**
     * Create a new element and optionally set attributes.
     *
     * @param string $name
     * @param \Titon\Type\Xml\AttributeMap $attributes
     * @param string $namespace
     */
    public function __construct(string $name, AttributeMap $attributes = Map {}, string $namespace = '') {
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
     * @param \Titon\Type\Xml\Element $child
     * @return $this
     */
    public function addChild(Element $child): this {
        $child->setParent($this);

        $this->children[] = $child;

        return $this;
    }

    /**
     * Add multiple children to this element.
     *
     * @param \Titon\Type\Xml\ElementList $children
     * @return $this
     */
    public function addChildren(ElementList $children): this {
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
        return count($this->children);
    }

    /**
     * Format attributes for use in an XML document.
     *
     * @param \Titon\Type\Xml\AttributeMap $attributes
     * @return string
     */
    public function formatAttributes(AttributeMap $attributes): string {
        $xml = '';

        foreach ($attributes as $key => $value) {
            $xml .= sprintf(' %s="%s"', Xml::formatName($key), Sanitize::escape($value));
        }

        return $xml;
    }

    /**
     * Format namespaces as attributes for use in an XML document.
     *
     * @param \Titon\Type\Xml\NamespaceMap $namespaces
     * @return string
     */
    public function formatNamespaces(NamespaceMap $namespaces): string {
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
     * @return \Titon\Type\Xml\AttributeMap
     */
    public function getAttributes(): AttributeMap {
        return $this->attributes;
    }

    /**
     * Return a type casted value.
     *
     * @return mixed
     */
    public function getBoxedValue(): mixed {
        return Xml::box($this->getValueWithoutCdata());
    }

    /**
     * Returns the first child that matches the defined name.
     *
     * @param string $name
     * @return \Titon\Type\Xml\Element
     */
    public function getChild(string $name): ?Element {
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
     * @return \Titon\Type\Xml\ElementList
     */
    public function getChildren(): ElementList {
        return $this->children;
    }

    /**
     * Return a list of children with the defined name.
     *
     * @return \Titon\Type\Xml\ElementList
     */
    public function getChildrenByName(string $name): ElementList {
        return $this->getChildren()->filter( $item ==> $item->getName() === $name );
    }

    /**
     * Return the opening declaration attributes.
     *
     * @return \Titon\Type\Xml\AttributeMap
     */
    public function getDeclaration(): AttributeMap {
        return $this->declaration;
    }

    /**
     * Return an iterator.
     *
     * @return Iterator<Element>
     */
    public function getIterator(): Iterator<Element> {
        return $this->getChildren()->getIterator();
    }

    /**
     * Return the name of the element.
     *
     * @return string
     */
    public function getName(): string {
        return $this->name;
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
     * @return \Titon\Type\Xml\NamespaceMap
     */
    public function getNamespaces(): NamespaceMap {
        return $this->namespaces;
    }

    /**
     * Return all attributes for a specific namespace.
     *
     * @param string $namespace
     * @return \Titon\Type\Xml\AttributeMap
     */
    public function getNamespaceAttributes(string $namespace): AttributeMap {
        return $this->getAttributes()->filterWithKey( ($key, $value) ==> strpos($key, $namespace . ':') === 0 );
    }

    /**
     * Return all children for a specific namespace.
     *
     * @param string $namespace
     * @return \Titon\Type\Xml\ElementList
     */
    public function getNamespaceChildren(string $namespace): ElementList {
        return $this->getChildren()->filter( $item ==> strpos($item->getName(), $namespace . ':') === 0 );
    }

    /**
     * Return the parent element or null if it does not exist.
     *
     * @return \Titon\Type\Xml\Element
     */
    public function getParent(): ?Element {
        return $this->parent;
    }

    /**
     * Return the value. If CDATA was set, then the value will be wrapped in CDATA tags.
     *
     * @return string
     */
    public function getValue(): string {
        return $this->value;
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

        $this->attributes[$key] = Xml::unbox($value);

        return $this;
    }

    /**
     * Set multiple attributes.
     *
     * @param \Titon\Type\Xml\AttributeMap $attributes
     * @return $this
     */
    public function setAttributes(AttributeMap $attributes): this {
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
     * @param \Titon\Type\Xml\AttributeMap $attributes
     * @return $this
     */
    public function setDeclaration(string $version = '1.0', string $encoding = 'UTF-8', AttributeMap $attributes = Map {}): this {
        $attributes['version'] = $version;
        $attributes['encoding'] = $encoding;

        $this->declaration->setAll($attributes);

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
        $name = Xml::formatName($name);

        if ($namespace) {
            $name = $namespace . ':' . $name;
        }

        $this->name = $name;

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
        $this->namespaces[$key] = $value;

        return $this;
    }

    /**
     * Set multiple namespaces.
     *
     * @param \Titon\Type\Xml\NamespaceMap $namespaces
     * @return $this
     */
    public function setNamespaces(NamespaceMap $namespaces): this {
        foreach ($namespaces as $key => $value) {
            $this->setNamespace($key, $value);
        }

        return $this;
    }

    /**
     * Set the parent element.
     *
     * @param \Titon\Type\Xml\Element $element
     * @return $this
     */
    public function setParent(Element $element): this {
        $this->parent = $element;

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
        $value = Xml::unbox($value);

        if ($cdata) {
            $value = '<![CDATA[' . PHP_EOL . $value . PHP_EOL . ']]>';
        }

        $this->value = $value;

        return $this;
    }

    /**
     * Return the element as a nested map structure.
     *
     * @param bool $includeRoot
     * @return \Titon\Type\XmlMap
     */
    public function toMap(bool $includeRoot = true): XmlMap {
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

        if ($includeRoot && $this->isRoot()) {
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
        if (!$this->hasChildren() && $this->getValue() === '') {
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
