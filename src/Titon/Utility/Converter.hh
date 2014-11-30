<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;
use Titon\Type\Contract\Jsonable;
use Titon\Type\Contract\Mapable;
use Titon\Type\Contract\Vectorable;
use Titon\Type\Contract\Xmlable;
use \JsonSerializable;
use \SimpleXMLElement;
use \Indexish;

/**
 * A class that handles the detection and conversion of certain resource formats / content types into other formats.
 * The current formats are supported: XML, JSON, Map, Vector, Object, Serialized
 *
 * @package Titon\Utility
 */
class Converter {
    use Macroable;

    /** Disregard XML attributes and only return the value */
    const int XML_NONE = 0;

    /** Merge attributes and the value into a single dimension; the values key will be "value" */
    const int XML_MERGE = 1;

    /** Group the attributes into a key "attributes" and the value into a key of "value" */
    const int XML_GROUP = 2;

    /** Attributes will only be returned */
    const int XML_ATTRIBS = 3;

    /**
     * Merge a map into an existing XML object.
     *
     * @param \SimpleXMLElement $xml
     * @param Map<mixed, mixed> $map
     * @return \SimpleXMLElement
     */
    public static function buildXml(SimpleXMLElement $xml, Map<mixed, mixed> $map): SimpleXMLElement {
        foreach ($map as $key => $value) {

            // XML_NONE
            if (!$value instanceof Collection) {
                $xml->addChild($key, static::unbox($value));

            // Multiple nodes of the same name
            } else if ($value instanceof Vector) {
                foreach ($value as $kValue) {
                    if ($kValue instanceof Collection) {
                        static::buildXml($xml, Map {$key => $kValue});
                    } else {
                        $xml->addChild($key, static::unbox($kValue));
                    }
                }

            // Single node with value and or attributes
            } else if ($value instanceof Map) {

                // XML_GROUP
                if ($value->contains('attributes')) {
                    if (!$value->contains('value')) {
                        $value['value'] = null;
                    }

                    $attributes = $value['attributes'];
                    $raw = $value['value'];

                    if ($raw instanceof Map) {
                        $node = $xml->addChild($key);
                        static::buildXml($node, $raw);

                    } else {
                        $node = $xml->addChild($key, static::unbox($raw));
                    }

                    if ($attributes instanceof KeyedTraversable) {
                        foreach ($attributes as $aKey => $aValue) {
                            $node->addAttribute($aKey, static::unbox($aValue));
                        }
                    }

                // XML_MERGE
                } else if ($value->contains('value')) {
                    $node = $xml->addChild($key, $value['value']);
                    $value->remove('value');

                    foreach ($value as $aKey => $aValue) {
                        $node->addAttribute($aKey, static::unbox($aValue));
                    }

                // XML_ATTRIBS
                } else {
                    $node = $xml->addChild($key);

                    foreach ($value as $aKey => $aValue) {
                        if ($aValue instanceof Collection) {
                            static::buildXml($node, Map {$aKey => $aValue});
                        } else {
                            $node->addChild($aKey, static::unbox($aValue));
                        }
                    }
                }

            }
        }

        return $xml;
    }

    /**
     * Convert a map to an XML string.
     *
     * @param Map<mixed, mixed> $map
     * @param string $root
     * @return string
     */
    public static function mapToXml(Map<mixed, mixed> $map, string $root): string {
        $xml = simplexml_load_string('<?xml version="1.0" encoding="utf-8"?><' . $root . '></' . $root . '>');
        $response = static::buildXml($xml, $map);

        return trim($response->asXML());
    }

    /**
     * Convert a SimpleXML object into a map.
     *
     * @param SimpleXMLElement $xml
     * @param int $format
     * @return Map<string, mixed>
     */
    public static function xmlToMap(SimpleXMLElement $xml, int $format = self::XML_GROUP): Map<string, mixed> {
        $map = Map {};

        /** @type SimpleXMLElement $node */
        foreach ($xml->children() as $element => $node) {
            $data = Map {};
            $hasChildren = $node->children()->count();

            if (!$node->attributes() || $format === self::XML_NONE) {
                if ($hasChildren) {
                    $data = static::xmlToMap($node, $format);
                } else {
                    $data = static::autobox((string) $node);
                }

            } else {
                switch ($format) {
                    case self::XML_GROUP:
                        if ($hasChildren) {
                            $data['value'] = static::xmlToMap($node, $format);
                        } else {
                            $data['value'] = static::autobox((string) $node);
                        }

                        $attributes = Map {};

                        foreach ($node->attributes() as $attr => $value) {
                            $attributes[$attr] = static::autobox((string) $value);
                        }

                        $data['attributes'] = $attributes;
                    break;

                    case self::XML_MERGE:
                        if ($hasChildren) {
                            $data = static::xmlToMap($node, $format);
                        } else {
                            $data['value'] = static::autobox((string) $node);
                        }
                    // FALLTHROUGH

                    case self::XML_ATTRIBS:
                        foreach ($node->attributes() as $attr => $value) {
                            $data[$attr] = static::autobox((string) $value);
                        }
                    break;
                }
            }

            // Append the node or set a value
            $element = (string) $element;
            $childNode = $xml->{$element};

            if ($childNode && count($childNode) > 1) {
                if (!$map->contains($element)) {
                    $map[$element] = Vector {};
                }

                $map[$element][] = $data;
            } else {
                $map[$element] = $data;
            }
        }

        return $map;
    }

}
