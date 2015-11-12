<?hh // strict
namespace Titon\Test\Stub\Http;

use Titon\Http\AbstractResponse;
use Titon\Http\Bag\HeaderBag;

class ResponseStub extends AbstractResponse {

    public function getHeaderBag(): HeaderBag {
        return $this->headers;
    }

}
