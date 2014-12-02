<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

use Titon\Type\Exception\MissingFileException;
use \SimpleXMLElement;

type XmlMap = Map<string, mixed>;

/**
 * The XmlDocument class provides helper methods for XML parsing and building as well as static methods
 * for generating XmlElement trees based on maps and raw XML files.
 *
 * @package Titon\Type
 */
class XmlDocument {

    /**
     * Box a value by type casting it from a string to a scalar.
     *
     * @param string $value
     * @return mixed
     */
    public static function box(string $value): mixed {
        if (is_numeric($value)) {
            if (strpos($value, '.') !== false) {
                return (float) $value;

            } else {
                return (int) $value;
            }

        } else if ($value === 'true' || $value === 'false') {
            return ($value === 'true');
        }

        return (string) $value;
    }

    /**
     * Unbox values by type casting to a string equivalent.
     *
     * @param mixed $value
     * @return string
     */
    public static function unbox(mixed $value): string {
        if (is_bool($value)) {
            return $value ? 'true' : 'false';
        }

        return (string) $value;
    }

    /**
     * Format an element or attribute name for converting to camel case.
     * If the element starts with a number, prefix it with an underscore.
     *
     * @return string
     */
    public static function formatName(string $name): string {
        $name = preg_replace('/[^a-z0-9:\.\-_]+/i', '', $name);
        $firstChar = mb_substr($name, 0, 1);

        if (is_numeric($firstChar) || $firstChar === '-' || $firstChar === '.') {
            $name = '_' . $name;
        }

        return $name;
    }

    /**
     * Transform a structure consisting of maps and vectors into an XmlElement tree.
     *
     * @param string $root
     * @param \Titon\Type\XmlMap $map
     * @return \Titon\Type\XmlElement
     */
    public static function fromMap(string $root = 'root', XmlMap $map): XmlElement {
        $root = new XmlElement($root);

        foreach ($map as $key => $value) {
            static::_createElement($root, $key, $value);
        }

        return $root;
    }

    /**
     * Transform a string representation of an XML document into an XmlElement tree.
     *
     * @param string $string
     * @return \Titon\Type\XmlElement
     */
    public static function fromString(string $string): XmlElement {
        return static::_convertSimpleXml(simplexml_load_string($string));
    }

    /**
     * Load an XML file from the file system and transform it into an XmlElement tree.
     *
     * @param string $path
     * @return \Titon\Type\XmlElement
     * @throws \Titon\Type\Exception\MissingFileException
     */
    public static function fromFile(string $path): XmlElement {
        if (file_exists($path)) {
            return static::fromString(file_get_contents($path));
        }

        throw new MissingFileException(sprintf('File %s does not exist', $path));
    }

    protected static function _createElement(XmlElement $parent, string $key, mixed $value): void {

        // One of two things:
        // An element that contains other children
        // An element that has attributes
        if ($value instanceof Map) {

            // An element with a value and attributes
            if ($value->contains('value')) {
                $attributes = $value;
                $value = $value['value'];
                $cdata = (bool) $attributes->get('cdata');

                $attributes->remove('value');
                $attributes->remove('cdata');

                $parent->addChild( (new XmlElement($key, $attributes))->setValue($value, $cdata) );

            // A child
            } else {
                $parent->addChild( static::fromMap($key, $value) );
            }

        // Multiple children with the same element name
        } else if ($value instanceof Vector) {
            foreach ($value as $item) {
                static::_createElement($parent, $key, $item);
            }

        // Child element with a value
        } else {
            $parent->addChild( (new XmlElement($key))->setValue($value) );
        }
    }

    protected static function _convertSimpleXml(SimpleXMLElement $xml): XmlElement {
        $element = new XmlElement($xml->getName());

        // Set attributes
        if ($attributes = $xml->attributes()) {
            foreach ($attributes as $key => $value) {
                $element->setAttribute((string) $key, (string) $value);
            }
        }

        // Set namespaces
        if ($namespaces = $xml->getNamespaces()) {
            foreach ($namespaces as $key => $value) {
                $element->setNamespace((string) $key, (string) $value);
            }
        }

        // Set value
        if ($value = (string) $xml) {
            $element->setValue($value);
        }

        // Add children
        foreach ($xml->children() as $child) {
            $element->addChild( static::_convertSimpleXml($child) );
        }

        return $element;
    }

}