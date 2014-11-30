<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

type XmlMap = Map<string, mixed>;

/**
 * The XmlDocument class provides helper methods for XML parsing and building as well as static methods
 * for generating XmlElement trees based on maps and raw XML files.
 *
 * @package Titon\Type
 */
class XmlDocument {

    /**
     * Autobox a value by type casting it from a string to a scalar.
     *
     * @param string $value
     * @return mixed
     */
    public static function autobox(string $value): mixed {
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

    public static function fromString(string $root = 'root', string $string): XmlElement {

    }

    public static function fromFile(string $root = 'root', string $string): XmlElement {

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

}