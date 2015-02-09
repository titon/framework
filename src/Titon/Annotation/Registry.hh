<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Annotation;

use Titon\Common\ArgumentList;
use Titon\Annotation\Exception\InvalidClassException;
use Titon\Annotation\Exception\MissingAnnotationException;
use ReflectionClass;

class Registry {

    /**
     * Map annotation names to their class names.
     *
     * @var \Titon\Annotation\RegistryMap
     */
    protected static RegistryMap $annotations = Map {};

    /**
     * Instantiate a new annotation class mapped by name and pass an array of arguments to the constructor.
     * The arguments are usually parsed from the <<>> attribute declaration.
     *
     * @param string $name
     * @param \Titon\Common\ArgumentList $args
     * @return Annotation
     */
    public static function factory(string $name, ArgumentList $args): Annotation {
        $annos = static::$annotations;

        if ($annos->contains($name)) {

            /** @var \Titon\Annotation\Annotation $object */
            $object = (new ReflectionClass($annos[$name]))->newInstanceArgs($args);
            $object->setName($name);

            return $object;
        }

        throw new MissingAnnotationException(sprintf('Annotation mapping %s does not exist', $name));
    }

    /**
     * Map an annotation class to a unique name. If the class does not extend the
     * Titon\Annotation\Annotation class, throw an exception.
     *
     * @param string $name
     * @param string $class
     */
    public static function map(string $name, string $class): void {
        if (!is_a($class, 'Titon\Annotation\Annotation', true)) {
            throw new InvalidClassException(sprintf('%s must extend the Titon\Annotation\Annotation class', $class));
        }

        static::$annotations[$name] = $class;
    }

}
