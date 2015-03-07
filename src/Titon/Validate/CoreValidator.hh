<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Validate;

/**
 * Core instanceable class for providing validation.
 *
 * @package Titon\Validate
 */
class CoreValidator extends AbstractValidator {

    /**
     * Store the data to validate and load the default set of constraints.
     *
     * @param \Titon\Validate\DataMap $data
     */
    public function __construct(DataMap $data = Map {}) {
        parent::__construct($data);

        $this->addConstraintProvider(new Constraint());
    }

}
