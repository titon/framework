<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Annotation;

class Annotation {

    /**
     * The annotation name which is set when factoried.
     *
     * @var string
     */
    protected string $name = '';

    /**
     * Return the annotation name.
     *
     * @return string
     */
    final public function getName(): string {
        return $this->name;
    }

    /**
     * Set the annotation name.
     *
     * @param string $name
     * @return $this
     */
    final public function setName(string $name): this {
        $this->name = $name;

        return $this;
    }

}
