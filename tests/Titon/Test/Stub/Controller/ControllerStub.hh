<?hh // strict
namespace Titon\Test\Stub\Controller;

use Titon\Controller\AbstractController;
use Titon\Http\Server\ResponseFactory;
use Titon\Http\Stream\MemoryStream;

class ControllerStub extends AbstractController {

    public string $value = '';

    public function actionWithArgs(mixed $arg1, mixed $arg2 = 0): mixed {
        return strval((int) $arg1 + (int) $arg2);
    }

    public function actionNoArgs(): mixed {
        return 'actionNoArgs';
    }

    public function _actionPseudoPrivate(): mixed {
        return 'wontBeCalled';
    }

    protected function actionProtected(): mixed {
        return 'wontBeCalled';
    }

    private function actionPrivate(): mixed {
        return 'wontBeCalled';
    }

    public function returnsStream(): mixed {
        return new MemoryStream('returnsStream');
    }

    public function returnsResponse(): mixed {
        return ResponseFactory::json(['returnsResponse']);
    }

    public function returnsNothing(): mixed {
        return null;
    }

    public function viewPath(string $action): string {
        return $this->buildViewPath($action);
    }

}
