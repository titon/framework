<?hh // strict
/**
 * @author Alex Phillips <exonintrendo@gmail.com>
 * Date: 2/9/15
 * Time: 5:19 PM
 */

namespace Titon\Context\Definition;

interface DefinitionInterface
{
    public function with(...$arguments);

    public function create(...$arguments);
}