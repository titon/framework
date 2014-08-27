<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;
use Titon\Type\Contract\Arrayable;
use Titon\Type\Contract\Jsonable;
use Titon\Type\Contract\Xmlable;
use \JsonSerializable;
use \Serializable;
use \SimpleXmlElement;

/**
 * A class that handles the detection and conversion of certain resource formats / content types into other formats.
 * The current formats are supported: XML, JSON, Array, Object, Serialized
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
     * Autobox a value by type casting it.
     *
     * @param mixed $value
     * @return mixed
     */
    public static function autobox(mixed $value): mixed {
        if (is_numeric($value)) {
            if (strpos($value, '.') !== false) {
                return (float) $value;

            } else {
                return (int) $value;
            }
        } else if (is_bool($value)) {
            return (bool) $value;

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
     * Returns a string for the detected type.
     *
     * @param mixed $data
     * @return string
     */
    public static function is(mixed $data): string {
        if (static::isArray($data)) {
            return 'array';

        } else if (static::isObject($data)) {
            return 'object';

        } else if (static::isJson($data)) {
            return 'json';

        } else if (static::isSerialized($data)) {
            return 'serialized';

        } else if (static::isXml($data)) {
            return 'xml';
        }

        // Check within macros
        foreach (static::getMacros() as $key => $macro) {
            if (preg_match('/^is/', $key) && $macro($data)) {
                return strtolower(preg_replace('/^is/', '', $key));
            }
        }

        // Attempt other types
        return strtolower(gettype($data));
    }

    /**
     * Check to see if data passed is an array.
     *
     * @param mixed $data
     * @return bool
     */
    public static function isArray(mixed $data): bool {
        return is_array($data);
    }

    /**
     * Check to see if data passed is a JSON object.
     *
     * @param mixed $data
     * @return bool
     */
    public static function isJson(mixed $data): bool {
        if (!is_string($data) || !$data) {
            return false;
        }

        $json = @json_decode($data, true);

        return (json_last_error() === JSON_ERROR_NONE && $json !== null);
    }

    /**
     * Check to see if data passed is an object.
     *
     * @param mixed $data
     * @return bool
     */
    public static function isObject(mixed $data): bool {
        return is_object($data);
    }

    /**
     * Check to see if data passed has been serialized.
     *
     * @param mixed $data
     * @return bool
     */
    public static function isSerialized(mixed $data): bool {
        if (!is_string($data) || !$data) {
            return false;
        }

        return (@unserialize($data) !== false);
    }

    /**
     * Check to see if data passed is an XML document.
     *
     * @param mixed $data
     * @return bool
     */
    public static function isXml(mixed $data): bool {
        // Do manually checks on the string since HHVM blows up
        if (!is_string($data) || substr($data, 0, 5) !== '<?xml') {
            return false;
        }

        return (@simplexml_load_string($data) instanceof SimpleXMLElement);
    }

    /**
     * Transforms a resource into an array.
     *
     * @param mixed $resource
     * @param bool $recursive
     * @return array
     */
    public static function toArray(mixed $resource, bool $recursive = false): array {
        if ($resource instanceof Arrayable) {
            $resource = $resource->toArray();

        } else if (static::isArray($resource)) {
            return $recursive ? static::buildArray($resource) : $resource;

        } else if (static::isObject($resource)) {
            return method_exists($resource, 'toArray') ? $resource->toArray() : static::buildArray($resource);

        } else if (static::isJson($resource)) {
            $resource = json_decode($resource, true);

        } else if (static::isSerialized($resource)) {
            $resource = unserialize($resource);

        } else if (static::isXml($resource)) {
            $resource = static::xmlToArray(simplexml_load_string($resource));
        }

        return (array) $resource;
    }

    /**
     * Transforms a resource into a JSON object.
     *
     * @param mixed $resource
     * @param int $options
     * @return string
     */
    public static function toJson(mixed $resource, int $options = 0): string {
        if ($resource instanceof JsonSerializable) {
            // pass-through

        } else if ($resource instanceof Jsonable) {
            return $resource->toJson($options);

        } else if (static::isJson($resource)) {
            return $resource;

        } else if (static::isObject($resource)) {
            $resource = static::buildArray($resource);

        } else if (static::isXml($resource)) {
            $resource = static::xmlToArray(simplexml_load_string($resource));

        } else if (static::isSerialized($resource)) {
            $resource = unserialize($resource);
        }

        return json_encode($resource, $options);
    }

    /**
     * Transform a resource into a map.
     * If the value is an array or traversable object, pass it to the map.
     * If it is any other value, wrap in array first.
     *
     * @param mixed $resouce
     * @return Map<mixed, mixed>
     */
    public static function toMap(mixed $resource): Map<mixed, mixed> {
        if (!$resource instanceof Traversable) {
            return new Map([$resource]);
        }

        $map = new Map();

        foreach ($resource as $key => $value) {
            if ($value instanceof Traversable) {
                $map[$key] = static::toMap($value);
            } else {
                $map[$key] = $value;
            }
        }

        return $map;
    }

    /**
     * Transforms a resource into a serialized form.
     *
     * @param mixed $resource
     * @return string
     */
    public static function toSerialize(mixed $resource): string {
        if ($resource instanceof Serializable) {
            // pass-through
        } else {
            $resource = static::toArray($resource);
        }

        return serialize($resource);
    }

    /**
     * Transform a resource into a vector.
     * If the value is an array or traversable object, pass it to the vector.
     * If it is any other value, wrap in array first.
     *
     * @param mixed $resouce
     * @return Vector<mixed>
     */
    public static function toVector(mixed $resource): Vector<mixed> {
        if (!$resource instanceof Traversable) {
            return new Vector([$resource]);
        }

        $vector = new Vector();

        foreach ($resource as $value) {
            if ($value instanceof Traversable) {
                $vector[] = static::toVector($value);
            } else {
                $vector[] = $value;
            }
        }

        return $vector;
    }

    /**
     * Transforms a resource into an XML document.
     *
     * @param mixed $resource
     * @param string $root
     * @return string
     */
    public static function toXml(mixed $resource, string $root = 'root'): string {
        if ($resource instanceof Xmlable) {
            return $resource->toXml($root);
        }

        return static::arrayToXml(static::toArray($resource, true), $root);
    }

    /**
     * Turn an object into an array. Alternative to array_map magic.
     *
     * @param object|array $object
     * @return array
     */
    public static function buildArray(mixed $object): array {
        $array = [];

        foreach ($object as $key => $value) {
            if (is_object($value) || is_array($value)) {
                $array[$key] = static::buildArray($value);

            } else if ($value instanceof Collection) {
                $array[$key] = $value->toArray();

            } else {
                $array[$key] = static::autobox($value);
            }
        }

        return $array;
    }

    /**
     * Turn an array into an XML document. Alternative to array_map magic.
     *
     * @param \SimpleXMLElement $xml
     * @param array $array
     * @return \SimpleXMLElement
     */
    public static function buildXml(SimpleXMLElement &$xml, array $array): SimpleXmlElement {
        if (!is_array($array)) {
            return $xml;
        }

        foreach ($array as $key => $value) {

            // XML_NONE
            if (!is_array($value)) {
                $xml->addChild($key, static::unbox($value));
                continue;
            }

            // Multiple nodes of the same name
            if (Traverse::isNumeric(new Vector(array_keys($value)))) {
                foreach ($value as $kValue) {
                    if (is_array($kValue)) {
                        static::buildXml($xml, [$key => $kValue]);
                    } else {
                        $xml->addChild($key, static::unbox($kValue));
                    }
                }

            // XML_GROUP
            } else if (isset($value['attributes'])) {
                if (!isset($value['value'])) {
                    $value['value'] = null;
                }

                if (is_array($value['value'])) {
                    $node = $xml->addChild($key);
                    static::buildXml($node, $value['value']);
                } else {
                    $node = $xml->addChild($key, static::unbox($value['value']));
                }

                if (isset($value['attributes'])) {
                    foreach ($value['attributes'] as $aKey => $aValue) {
                        $node->addAttribute($aKey, static::unbox($aValue));
                    }
                }

            // XML_MERGE
            } else if (isset($value['value'])) {
                $node = $xml->addChild($key, $value['value']);
                unset($value['value']);

                if (isset($value)) {
                    foreach ($value as $aKey => $aValue) {
                        $node->addAttribute($aKey, static::unbox($aValue));
                    }
                }

            // XML_ATTRIBS
            } else {
                $node = $xml->addChild($key);

                if (isset($value)) {
                    foreach ($value as $aKey => $aValue) {
                        if (is_array($aValue)) {
                            static::buildXml($node, [$aKey => $aValue]);
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
     * Convert an array to an XML string.
     *
     * @param array $array
     * @param string $root
     * @return string
     */
    public static function arrayToXml(array $array, string $root): string {
        $xml = simplexml_load_string('<?xml version="1.0" encoding="utf-8"?><' . $root . '></' . $root . '>');
        $response = static::buildXml($xml, $array);

        return trim($response->asXML());
    }

    /**
     * Convert a SimpleXML object into an array.
     *
     * @param SimpleXMLElement $xml
     * @param int $format
     * @return array
     */
    public static function xmlToArray(SimpleXMLElement $xml, int $format = self::XML_GROUP): mixed {
        if (count($xml->children()) <= 0) {
            return static::autobox((string) $xml);
        }

        $array = [];

        /** @type SimpleXMLElement $node */
        foreach ($xml->children() as $element => $node) {
            $data = [];
            $children = $node->children();

            if (!isset($array[$element])) {
                $array[$element] = '';
            }

            if (!$node->attributes() || $format === self::XML_NONE) {
                $data = static::xmlToArray($node, $format);

            } else {
                switch ($format) {
                    case self::XML_GROUP:
                        $data = [
                            'value' => static::autobox((string) $node),
                            'attributes' => []
                        ];

                        if (count($children) > 0) {
                            $data['value'] = static::xmlToArray($node, $format);
                        }

                        foreach ($node->attributes() as $attr => $value) {
                            $data['attributes'][$attr] = static::autobox((string) $value);
                        }
                    break;

                    case self::XML_MERGE:
                        if (count($children) > 0) {
                            $data = $data + static::xmlToArray($node, $format);
                        } else {
                            $data['value'] = static::autobox((string) $node);
                        }
                    /* fall-through */

                    case self::XML_ATTRIBS:
                        foreach ($node->attributes() as $attr => $value) {
                            $data[$attr] = static::autobox((string) $value);
                        }
                    break;
                }
            }

            if (count($xml->{$element}) > 1) {
                $array[$element][] = $data;
            } else {
                $array[$element] = $data;
            }
        }

        return $array;
    }

}
