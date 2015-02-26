<?hh
namespace Titon\Test\Stub\Annotation;

<<Foo, Bar('abc', 123)>>
class ReaderStub {

    <<Baz(['a' => 1])>>
    public function hasAnno(): void {}

    public function noAnno(): void {}
}
