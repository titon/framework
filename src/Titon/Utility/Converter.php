<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

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
class Converter extends Macro {

    /** Disregard XML attributes and only return the value */
    const XML_NONE = 0;

    /** Merge attributes and the value into a single dimension; the values key will be "value" */
    const XML_MERGE = 1;

    /** Group the attributes into a key "attributes" and the value into a key of "value" */
    const XML_GROUP = 2;

    /** Attributes will only be returned */
    const XML_ATTRIBS = 3;

    /**
     * Autobox a value by type casting it.
     *
     * @param mixed $value
     * @return mixed
     */
    public static function autobox($value) {
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
    public static function unbox($value) {
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
    public static function is($data) {
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
        foreach (self::$_macros[get_called_class()] as $key => $macro) {
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
    public static function isArray($data) {
        return is_array($data);
    }

    /**
     * Check to see if data passed is a JSON object.
     *
     * @param mixed $data
     * @return bool
     */
    public static function isJson($data) {
        if (!is_string($data) || empty($data)) {
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
    public static function isObject($data) {
        return is_object($data);
    }

    /**
     * Check to see if data passed has been serialized.
     *
     * @param mixed $data
     * @return bool
     */
    public static function isSerialized($data) {
        if (!is_string($data) || empty($data)) {
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
    public static function isXml($data) {
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
    public static function toArray($resource, $recursive = false) {
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
    public static function toJson($resource, $options = 0) {
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
     * Transforms a resource into an object.
     *
     * @param mixed $resource
     * @param bool $recursive
     * @return object
     */
    public static function toObject($resource, $recursive = false) {
        if (static::isObject($resource)) {
            if (!$recursive) {
                return $resource;
            }

        } else if (static::isArray($resource)) {
            // Continue

        } else if (static::isJson($resource)) {
            $resource = json_decode($resource, true);

        } else if (static::isSerialized($resource)) {
            $resource = unserialize($resource);

        } else if (static::isXml($resource)) {
            $resource = static::xmlToArray(simplexml_load_string($resource));
        }

        return static::buildObject($resource);
    }

    /**
     * Transforms a resource into a serialized form.
     *
     * @param mixed $resource
     * @return string
     */
    public static function toSerialize($resource) {
        if ($resource instanceof Serializable) {
            // pass-through
        } else {
            $resource = static::toArray($resource);
        }

        return serialize($resource);
    }

    /**
     * Transforms a resource into an XML document.
     *
     * @param mixed $resource
     * @param string $root
     * @return string
     */
    public static function toXml($resource, $root = 'root') {
        if ($resource instanceof Xmlable) {
            return $resource->toXml($root);

        } else if ($array = static::toArray($resource, true)) {
            $xml = simplexml_load_string('<?xml version="1.0" encoding="utf-8"?><' . $root . '></' . $root . '>');
            $response = static::buildXml($xml, $array);

            return trim($response->asXML());
        }

        return null;
    }

    /**
     * Turn an object into an array. Alternative to array_map magic.
     *
     * @param object|array $object
     * @return array
     */
    public static function buildArray($object) {
        $array = array();

        foreach ($object as $key => $value) {
            if (is_object($value) || is_array($value)) {
                $array[$key] = static::buildArray($value);
            } else {
                $array[$key] = static::autobox($value);
            }
        }

        return $array;
    }

    /**
     * Turn an array into an object. Alternative to array_map magic.
     *
     * @param array|object $array
     * @return object
     */
    public static function buildObject($array) {
        $obj = new \stdClass();

        foreach ($array as $key => $value) {
            if (is_array($value) || is_object($value)) {
                $obj->{$key} = static::buildObject($value);
            } else {
                $obj->{$key} = static::autobox($value);
            }
        }

        return $obj;
    }

    /**
     * Turn an array into an XML document. Alternative to array_map magic.
     *
     * @param \SimpleXMLElement $xml
     * @param array $array
     * @return \SimpleXMLElement
     */
    public static function buildXml(SimpleXMLElement &$xml, $array) {
        if (is_array($array)) {
            foreach ($array as $key => $value) {

                // XML_NONE
                if (!is_array($value)) {
                    $xml->addChild($key, static::unbox($value));
                    continue;
                }

                // Multiple nodes of the same name
                if (Hash::isNumeric(array_keys($value))) {
                    foreach ($value as $kValue) {
                        if (is_array($kValue)) {
                            static::buildXml($xml, array($key => $kValue));
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

                    if (!empty($value['attributes'])) {
                        foreach ($value['attributes'] as $aKey => $aValue) {
                            $node->addAttribute($aKey, static::unbox($aValue));
                        }
                    }

                // XML_MERGE
                } else if (isset($value['value'])) {
                    $node = $xml->addChild($key, $value['value']);
                    unset($value['value']);

                    if (!empty($value)) {
                        foreach ($value as $aKey => $aValue) {
                            if (is_array($aValue)) {
                                static::buildXml($node, array($aKey => $aValue));
                            } else {
                                $node->addAttribute($aKey, static::unbox($aValue));
                            }
                        }
                    }

                // XML_ATTRIBS
                } else {
                    $node = $xml->addChild($key);

                    if (!empty($value)) {
                        foreach ($value as $aKey => $aValue) {
                            if (is_array($aValue)) {
                                static::buildXml($node, array($aKey => $aValue));
                            } else {
                                $node->addChild($aKey, static::unbox($aValue));
                            }
                        }
                    }
                }
            }
        }

        return $xml;
    }

    /**
     * Convert a SimpleXML object into an array.
     *
     * @param SimpleXMLElement $xml
     * @param int $format
     * @return array
     */
    public static function xmlToArray(SimpleXMLElement $xml, $format = self::XML_GROUP) {
        if (count($xml->children()) <= 0) {
            return static::autobox((string) $xml);
        }

        $array = array();

        /** @type SimpleXMLElement $node */
        foreach ($xml->children() as $element => $node) {
            $data = array();
            $children = $node->children();

            if (!isset($array[$element])) {
                $array[$element] = '';
            }

            if (!$node->attributes() || $format === static::XML_NONE) {
                $data = static::xmlToArray($node, $format);

            } else {
                switch ($format) {
                    case static::XML_GROUP:
                        $data = array(
                            'value' => static::autobox((string) $node),
                            'attributes' => array()
                        );

                        if (count($children) > 0) {
                            $data['value'] = static::xmlToArray($node, $format);
                        }

                        foreach ($node->attributes() as $attr => $value) {
                            $data['attributes'][$attr] = static::autobox((string) $value);
                        }
                    break;

                    case static::XML_MERGE:
                        if (count($children) > 0) {
                            $data = $data + static::xmlToArray($node, $format);
                        } else {
                            $data['value'] = static::autobox((string) $node);
                        }
                    /* fall-through */

                    case static::XML_ATTRIBS:
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
