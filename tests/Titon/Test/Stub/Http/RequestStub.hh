<?hh // strict
namespace Titon\Test\Stub\Http;

use Titon\Http\AbstractRequest;
use Titon\Http\Bag\HeaderBag;

class RequestStub extends AbstractRequest {

    public function getHeaderBag(): HeaderBag {
        return $this->headers;
    }

}
