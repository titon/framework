<?hh // strict
namespace Titon\Test\Stub\Controller;

use Titon\Controller\Action\AbstractAction;

class ActionStub extends AbstractAction {

    public function get(): mixed {
        // UNSAFE
        $this->getController()->value = 'bar';

        return 'get';
    }

    public function post(): mixed {
        return 'post';
    }

    public function delete(): mixed {
        return 'delete';
    }

    public function put(): mixed {
        return 'put';
    }

    public function head(): mixed {
        // UNSAFE
        $this->getController()->value = 'baz';

        return 'head';
    }

}
