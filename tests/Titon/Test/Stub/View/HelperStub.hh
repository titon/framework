<?hh // strict
namespace Titon\Test\Stub\View;

use Titon\View\Helper\AbstractHelper;
use Titon\View\Helper\TagMap;

class HelperStub extends AbstractHelper {

    protected TagMap $tags = Map {
        'noattr' => '<tag>{body}</tag>',
        'nobody' => '<tag{attr} />',
        'custom' => '<tag {one} {two}>{three}</tag>',
        'default' => '<tag {0}>{1}</tag>{2}'
    };

}
