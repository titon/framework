<?hh // strict
namespace Titon\Test\Stub\Http;

use Titon\Http\AbstractMessage;
use Titon\Http\Bag\HeaderBag;

class MessageStub extends AbstractMessage {

    public function getHeaderBag(): HeaderBag {
        return $this->headers;
    }

}
