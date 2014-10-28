<?hh
namespace Titon\View\Helper;

use Titon\Model\Model;
use Titon\Test\TestCase;
use Titon\Utility\State\Get;
use Titon\Utility\State\Post;
use Titon\Utility\State\Server;

/**
 * @property \Titon\View\Helper\FormHelper $object
 */
class FormHelperTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $_SERVER['HTTP_USER_AGENT'] = 'browser';
        $_SERVER['REQUEST_METHOD'] = 'POST';
        $_POST['Test'] = [];

        Server::initialize($_SERVER);
        Post::initialize($_POST);

        $this->object = new FormHelper();
        $this->object->open('/');
    }

    public function testValueAttributeEscaping() {
        $this->assertEquals('<input id="text" name="text" type="text" value="">' . PHP_EOL, $this->object->text('text'));
        $this->assertEquals('<input id="text" name="text" type="text" value="Value with &quot;quotes&quot;">' . PHP_EOL, $this->object->text('text', 'Value with "quotes"'));
    }

    public function testValueOverrides() {
        if (!class_exists('Titon\Model\Model')) {
            $this->markTestSkipped('Test skipped; Please install titon/model via Composer');
        }

        $this->assertEquals('<input id="text" name="text" type="text" value="">' . PHP_EOL, $this->object->text('text'));

        // Value set through argument
        $this->assertEquals('<input id="text" name="text" type="text" value="default">' . PHP_EOL, $this->object->text('text', 'default'));

        // Value set through default attribute
        $this->assertEquals('<input id="text" name="text" type="text" value="default">' . PHP_EOL, $this->object->text('text', null, ['default' => 'default']));

        // Model value should take precedence before default value
        $this->object->setModel(new Model(['text' => 'model']));

        $this->assertEquals('<input id="model-text" name="text" type="text" value="model">' . PHP_EOL, $this->object->text('text', 'default'));

        // Request value should take highest precedence
        $_POST['text'] = 'request';
        Post::initialize($_POST);

        $this->assertEquals('<input id="model-text" name="text" type="text" value="request">' . PHP_EOL, $this->object->text('text', 'default'));

        $_POST = [];
        Post::initialize($_POST);

        $this->assertEquals('<input id="model-text" name="text" type="text" value="model">' . PHP_EOL, $this->object->text('text', 'default'));
    }

    public function testBinary() {
        $this->assertEquals('<input id="binary-hidden" name="binary" type="hidden" value="0">' . PHP_EOL .
            '<input id="binary" name="binary" type="checkbox" value="1">' . PHP_EOL, $this->object->binary('binary'));

        // With data
        $_POST['binary'] = 1;
        Post::initialize($_POST);

        $this->assertEquals('<input id="binary-hidden" name="binary" type="hidden" value="0">' . PHP_EOL .
            '<input checked="checked" id="binary" name="binary" type="checkbox" value="1">' . PHP_EOL, $this->object->binary('binary'));
    }

    public function testCheckbox() {
        $this->assertEquals('<input id="checkbox" name="checkbox" type="checkbox" value="1">' . PHP_EOL, $this->object->checkbox('checkbox'));

        // tiered depth
        $this->assertEquals('<input id="check-box" name="check[box]" type="checkbox" value="1">' . PHP_EOL, $this->object->checkbox('check.box'));

        // attributes
        $this->assertEquals('<input class="class-name" id="custom-id" name="checkbox" type="checkbox" value="1">' . PHP_EOL, $this->object->checkbox('checkbox', '1', Map {
            'class' => 'class-name',
            'id' => 'custom-id'
        }));

        // value
        $this->assertEquals('<input id="checkbox" name="checkbox" type="checkbox" value="yes">' . PHP_EOL, $this->object->checkbox('checkbox', 'yes'));

        // checked
        $this->assertEquals('<input checked="checked" id="checkbox" name="checkbox" type="checkbox" value="yes">' . PHP_EOL, $this->object->checkbox('checkbox', 'yes', Map {'default' => true}));

        // value with data
        $_POST['Test']['checkbox'] = 0;
        Post::initialize($_POST);

        $this->assertEquals('<input id="test-checkbox" name="Test[checkbox]" type="checkbox" value="1">' . PHP_EOL, $this->object->checkbox('Test.checkbox'));

        $_POST['Test']['checkbox'] = 1;
        Post::initialize($_POST);

        $this->assertEquals('<input checked="checked" id="test-checkbox" name="Test[checkbox]" type="checkbox" value="1">' . PHP_EOL, $this->object->checkbox('Test.checkbox'));
    }

    public function testCheckboxes() {
        $options = Vector {'red', 'blue', 'green'};

        // regular
        $this->assertEquals(Vector {
            '<input id="checkboxes-red" name="checkboxes[]" type="checkbox" value="red">' . PHP_EOL,
            '<input id="checkboxes-blue" name="checkboxes[]" type="checkbox" value="blue">' . PHP_EOL,
            '<input id="checkboxes-green" name="checkboxes[]" type="checkbox" value="green">' . PHP_EOL
        }, $this->object->checkboxes('checkboxes', $options));

        // default
        $this->assertEquals(Vector {
            '<input id="checkboxes-red" name="checkboxes[]" type="checkbox" value="red">' . PHP_EOL,
            '<input checked="checked" id="checkboxes-blue" name="checkboxes[]" type="checkbox" value="blue">' . PHP_EOL,
            '<input id="checkboxes-green" name="checkboxes[]" type="checkbox" value="green">' . PHP_EOL
        }, $this->object->checkboxes('checkboxes', $options, Map {'default' => 'blue'}));

        // multiple default
        $this->assertEquals(Vector {
            '<input checked="checked" id="checkboxes-red" name="checkboxes[]" type="checkbox" value="red">' . PHP_EOL,
            '<input checked="checked" id="checkboxes-blue" name="checkboxes[]" type="checkbox" value="blue">' . PHP_EOL,
            '<input id="checkboxes-green" name="checkboxes[]" type="checkbox" value="green">' . PHP_EOL
        }, $this->object->checkboxes('checkboxes', $options, Map {'default' => ['blue', 'red']}));

        // default with data
        $_POST['Test']['checkboxes'] = 'red';
        Post::initialize($_POST);

        $this->assertEquals(Vector {
            '<input checked="checked" id="test-checkboxes-red" name="Test[checkboxes][]" type="checkbox" value="red">' . PHP_EOL,
            '<input id="test-checkboxes-blue" name="Test[checkboxes][]" type="checkbox" value="blue">' . PHP_EOL,
            '<input id="test-checkboxes-green" name="Test[checkboxes][]" type="checkbox" value="green">' . PHP_EOL
        }, $this->object->checkboxes('Test.checkboxes', $options, Map {'default' => 'blue'}));

        // default with multiple data
        $_POST['Test']['checkboxes_multi'] = ['red', 'green'];
        Post::initialize($_POST);

        $this->assertEquals(Vector {
            '<input checked="checked" id="test-checkboxesmulti-red" name="Test[checkboxes_multi][]" type="checkbox" value="red">' . PHP_EOL,
            '<input id="test-checkboxesmulti-blue" name="Test[checkboxes_multi][]" type="checkbox" value="blue">' . PHP_EOL,
            '<input checked="checked" id="test-checkboxesmulti-green" name="Test[checkboxes_multi][]" type="checkbox" value="green">' . PHP_EOL
        }, $this->object->checkboxes('Test.checkboxes_multi', $options, Map {'default' => 'blue'}));
    }

    public function testClose() {
        $this->assertEquals('</form>' . PHP_EOL, $this->object->close());
    }

    public function testDay() {
        $this->assertEquals(
            '<select id="day" name="day">' . PHP_EOL .
            '<option value="1">1</option>' . PHP_EOL .
            '<option value="2">2</option>' . PHP_EOL .
            '<option value="3">3</option>' . PHP_EOL .
            '<option value="4">4</option>' . PHP_EOL .
            '<option value="5">5</option>' . PHP_EOL .
            '<option value="6">6</option>' . PHP_EOL .
            '<option value="7">7</option>' . PHP_EOL .
            '<option value="8">8</option>' . PHP_EOL .
            '<option value="9">9</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->day('day'));

        // format
        $this->assertEquals(
            '<select id="day" name="day">' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->day('day', Map {'dayFormat' => 'd'}));

        // default
        $this->assertEquals(
            '<select id="day" name="day">' . PHP_EOL .
            '<option value="1">1</option>' . PHP_EOL .
            '<option value="2">2</option>' . PHP_EOL .
            '<option value="3">3</option>' . PHP_EOL .
            '<option value="4">4</option>' . PHP_EOL .
            '<option value="5">5</option>' . PHP_EOL .
            '<option value="6">6</option>' . PHP_EOL .
            '<option value="7">7</option>' . PHP_EOL .
            '<option value="8">8</option>' . PHP_EOL .
            '<option value="9">9</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option selected="selected" value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->day('day', Map {'defaultDay' => 13}));

        // with data
        $_POST['Test']['day'] = 3;
        Post::initialize($_POST);

        $this->assertEquals(
            '<select id="test-day" name="Test[day]">' . PHP_EOL .
            '<option value="1">1</option>' . PHP_EOL .
            '<option value="2">2</option>' . PHP_EOL .
            '<option selected="selected" value="3">3</option>' . PHP_EOL .
            '<option value="4">4</option>' . PHP_EOL .
            '<option value="5">5</option>' . PHP_EOL .
            '<option value="6">6</option>' . PHP_EOL .
            '<option value="7">7</option>' . PHP_EOL .
            '<option value="8">8</option>' . PHP_EOL .
            '<option value="9">9</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->day('Test.day', Map {'defaultDay' => 13}));
    }

    public function testDate() {
        $this->assertEquals(
            '<select id="created-month" name="created[month]">' . PHP_EOL .
            '<option value="1">January</option>' . PHP_EOL .
            '<option value="2">February</option>' . PHP_EOL .
            '<option value="3">March</option>' . PHP_EOL .
            '<option value="4">April</option>' . PHP_EOL .
            '<option value="5">May</option>' . PHP_EOL .
            '<option value="6">June</option>' . PHP_EOL .
            '<option value="7">July</option>' . PHP_EOL .
            '<option value="8">August</option>' . PHP_EOL .
            '<option value="9">September</option>' . PHP_EOL .
            '<option value="10">October</option>' . PHP_EOL .
            '<option value="11">November</option>' . PHP_EOL .
            '<option value="12">December</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            ' <select id="created-day" name="created[day]">' . PHP_EOL .
            '<option value="1">1</option>' . PHP_EOL .
            '<option value="2">2</option>' . PHP_EOL .
            '<option value="3">3</option>' . PHP_EOL .
            '<option value="4">4</option>' . PHP_EOL .
            '<option value="5">5</option>' . PHP_EOL .
            '<option value="6">6</option>' . PHP_EOL .
            '<option value="7">7</option>' . PHP_EOL .
            '<option value="8">8</option>' . PHP_EOL .
            '<option value="9">9</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            ' <select id="created-year" name="created[year]">' . PHP_EOL .
            '<option value="2005">2005</option>' . PHP_EOL .
            '<option value="2006">2006</option>' . PHP_EOL .
            '<option value="2007">2007</option>' . PHP_EOL .
            '<option value="2008">2008</option>' . PHP_EOL .
            '<option value="2009">2009</option>' . PHP_EOL .
            '<option value="2010">2010</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->date('created', Map {
            'startYear' => 2005,
            'endYear' => 2010
        }));
    }

    public function testDateTime() {
        $this->object->use12Hour();

        $this->assertEquals(
            '<select id="created-month" name="created[month]">' . PHP_EOL .
            '<option value="1">January</option>' . PHP_EOL .
            '<option value="2">February</option>' . PHP_EOL .
            '<option value="3">March</option>' . PHP_EOL .
            '<option value="4">April</option>' . PHP_EOL .
            '<option value="5">May</option>' . PHP_EOL .
            '<option value="6">June</option>' . PHP_EOL .
            '<option value="7">July</option>' . PHP_EOL .
            '<option value="8">August</option>' . PHP_EOL .
            '<option value="9">September</option>' . PHP_EOL .
            '<option value="10">October</option>' . PHP_EOL .
            '<option value="11">November</option>' . PHP_EOL .
            '<option value="12">December</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            ' <select id="created-day" name="created[day]">' . PHP_EOL .
            '<option value="1">1</option>' . PHP_EOL .
            '<option value="2">2</option>' . PHP_EOL .
            '<option value="3">3</option>' . PHP_EOL .
            '<option value="4">4</option>' . PHP_EOL .
            '<option value="5">5</option>' . PHP_EOL .
            '<option value="6">6</option>' . PHP_EOL .
            '<option value="7">7</option>' . PHP_EOL .
            '<option value="8">8</option>' . PHP_EOL .
            '<option value="9">9</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            ' <select id="created-year" name="created[year]">' . PHP_EOL .
            '<option value="2005">2005</option>' . PHP_EOL .
            '<option value="2006">2006</option>' . PHP_EOL .
            '<option value="2007">2007</option>' . PHP_EOL .
            '<option value="2008">2008</option>' . PHP_EOL .
            '<option value="2009">2009</option>' . PHP_EOL .
            '<option value="2010">2010</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            ' - <select id="created-hour" name="created[hour]">' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            ':<select id="created-minute" name="created[minute]">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '<option value="32">32</option>' . PHP_EOL .
            '<option value="33">33</option>' . PHP_EOL .
            '<option value="34">34</option>' . PHP_EOL .
            '<option value="35">35</option>' . PHP_EOL .
            '<option value="36">36</option>' . PHP_EOL .
            '<option value="37">37</option>' . PHP_EOL .
            '<option value="38">38</option>' . PHP_EOL .
            '<option value="39">39</option>' . PHP_EOL .
            '<option value="40">40</option>' . PHP_EOL .
            '<option value="41">41</option>' . PHP_EOL .
            '<option value="42">42</option>' . PHP_EOL .
            '<option value="43">43</option>' . PHP_EOL .
            '<option value="44">44</option>' . PHP_EOL .
            '<option value="45">45</option>' . PHP_EOL .
            '<option value="46">46</option>' . PHP_EOL .
            '<option value="47">47</option>' . PHP_EOL .
            '<option value="48">48</option>' . PHP_EOL .
            '<option value="49">49</option>' . PHP_EOL .
            '<option value="50">50</option>' . PHP_EOL .
            '<option value="51">51</option>' . PHP_EOL .
            '<option value="52">52</option>' . PHP_EOL .
            '<option value="53">53</option>' . PHP_EOL .
            '<option value="54">54</option>' . PHP_EOL .
            '<option value="55">55</option>' . PHP_EOL .
            '<option value="56">56</option>' . PHP_EOL .
            '<option value="57">57</option>' . PHP_EOL .
            '<option value="58">58</option>' . PHP_EOL .
            '<option value="59">59</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            ':<select id="created-second" name="created[second]">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '<option value="32">32</option>' . PHP_EOL .
            '<option value="33">33</option>' . PHP_EOL .
            '<option value="34">34</option>' . PHP_EOL .
            '<option value="35">35</option>' . PHP_EOL .
            '<option value="36">36</option>' . PHP_EOL .
            '<option value="37">37</option>' . PHP_EOL .
            '<option value="38">38</option>' . PHP_EOL .
            '<option value="39">39</option>' . PHP_EOL .
            '<option value="40">40</option>' . PHP_EOL .
            '<option value="41">41</option>' . PHP_EOL .
            '<option value="42">42</option>' . PHP_EOL .
            '<option value="43">43</option>' . PHP_EOL .
            '<option value="44">44</option>' . PHP_EOL .
            '<option value="45">45</option>' . PHP_EOL .
            '<option value="46">46</option>' . PHP_EOL .
            '<option value="47">47</option>' . PHP_EOL .
            '<option value="48">48</option>' . PHP_EOL .
            '<option value="49">49</option>' . PHP_EOL .
            '<option value="50">50</option>' . PHP_EOL .
            '<option value="51">51</option>' . PHP_EOL .
            '<option value="52">52</option>' . PHP_EOL .
            '<option value="53">53</option>' . PHP_EOL .
            '<option value="54">54</option>' . PHP_EOL .
            '<option value="55">55</option>' . PHP_EOL .
            '<option value="56">56</option>' . PHP_EOL .
            '<option value="57">57</option>' . PHP_EOL .
            '<option value="58">58</option>' . PHP_EOL .
            '<option value="59">59</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            '<select id="created-meridiem" name="created[meridiem]">' . PHP_EOL .
            '<option value="am">AM</option>' . PHP_EOL .
            '<option value="pm">PM</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->dateTime('created', Map {
            'startYear' => 2005,
            'endYear' => 2010
        }));
    }

    public function testEmail() {
        $this->assertEquals('<input id="email" name="email" type="email" value="">' . PHP_EOL, $this->object->email('email'));

        // with data
        $this->assertEquals('<input id="test-email" name="Test[email]" type="email" value="">' . PHP_EOL, $this->object->email('Test.email'));

        $_POST['Test']['email'] = 'email@domain.com';
        Post::initialize($_POST);

        $this->assertEquals('<input id="test-email" name="Test[email]" type="email" value="email@domain.com">' . PHP_EOL, $this->object->email('Test.email'));
    }

    public function testFile() {
        $this->assertEquals('<input id="file" name="file" type="file">' . PHP_EOL, $this->object->file('file'));

        // no value
        $this->assertEquals('<input id="file" name="file" type="file">' . PHP_EOL, $this->object->file('file', Map {'value' => 'none'}));
    }

    public function testFormatID() {
        $this->assertEquals('key', $this->object->formatID('key'));
        $this->assertEquals('key-nested', $this->object->formatID('key.nested'));
        $this->assertEquals('key-nested-again', $this->object->formatID('key.nested.again'));
        $this->assertEquals('key-nested-again', $this->object->formatID('key.nested.again..'));
    }

    public function testFormatIDWithModel() {
        if (!class_exists('Titon\Model\Model')) {
            $this->markTestSkipped('Test skipped; Please install titon/model via Composer');
        }

        $this->object->setModel(new Model());

        $this->assertEquals('model-key', $this->object->formatID('key'));
        $this->assertEquals('model-key-nested', $this->object->formatID('key.nested'));
        $this->assertEquals('model-key-nested-again', $this->object->formatID('key.nested.again'));
        $this->assertEquals('model-key-nested-again', $this->object->formatID('key.nested.again..'));
    }

    public function testFormatName() {
        $this->assertEquals('key', $this->object->formatName('key'));
        $this->assertEquals('key[nested]', $this->object->formatName('key.nested'));
        $this->assertEquals('key[nested][again]', $this->object->formatName('key.nested.again'));
        $this->assertEquals('key[nested][again]', $this->object->formatName('key.nested.again..'));
        $this->assertEquals('key[nested][again][]', $this->object->formatName('key.nested.again.[]'));
        $this->assertEquals('key[nested][again][0]', $this->object->formatName('key.nested.again.[0]'));
    }

    public function testGetDefaultValue() {
        $this->assertEquals(null, $this->object->getDefaultValue(Map {}));
        $this->assertEquals('foo', $this->object->getDefaultValue(Map {'default' => 'foo'}));
    }

    public function testGetRequestValue() {
        $this->assertEquals(null, $this->object->getRequestValue('foo'));

        $_POST['foo'] = 'bar';
        Post::initialize($_POST);

        $this->assertEquals('bar', $this->object->getRequestValue('foo'));

        $_GET['get'] = '123';
        Get::initialize($_GET);

        $this->assertEquals('123', $this->object->getRequestValue('get'));

        $_POST['Test']['foo'] = 'baz';
        Post::initialize($_POST);

        $this->assertEquals('baz', $this->object->getRequestValue('Test.foo'));
    }

    public function testGetValueModel() {
        if (!class_exists('Titon\Model\Model')) {
            $this->markTestSkipped('Test skipped; Please install titon/model via Composer');
        }

        $model = new Model([
            'foo' => 'bar',
            'key' => 123,
            'Test' => ['foo' => 'baz']
        ]);

        $this->object->setModel($model);

        $this->assertEquals('bar', $this->object->getValue('foo'));
        $this->assertEquals('123', $this->object->getValue('key'));
        $this->assertEquals('baz', $this->object->getValue('Test.foo'));
    }

    public function testGetValue() {
        $this->assertEquals(null, $this->object->getValue('foo'));
        $this->assertEquals('bar', $this->object->getValue('foo', Map {'default' => 'bar'}));

        $_POST['foo'] = 'baz';
        Post::initialize($_POST);

        $this->assertEquals('baz', $this->object->getValue('foo', Map {'default' => 'bar'}));
    }

    public function testHidden() {
        $this->assertEquals('<input id="hidden" name="hidden" type="hidden" value="">' . PHP_EOL, $this->object->hidden('hidden'));
        $this->assertEquals('<input id="hidden" name="hidden" type="hidden" value="foobar">' . PHP_EOL, $this->object->hidden('hidden', 'foobar'));

        // with data
        $this->assertEquals('<input id="test-hidden" name="Test[hidden]" type="hidden" value="no">' . PHP_EOL, $this->object->hidden('Test.hidden', 'no'));

        $_POST['Test']['hidden'] = 'yes';
        Post::initialize($_POST);

        $this->assertEquals('<input id="test-hidden" name="Test[hidden]" type="hidden" value="yes">' . PHP_EOL, $this->object->hidden('Test.hidden', 'no'));
    }

    public function testHour() {
        $this->object->use12Hour();

        $this->assertEquals(
            '<select id="hour" name="hour">' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->hour('hour'));

        $this->assertEquals(
            '<select id="hour" name="hour" onchange="update(this);">' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option selected="selected" value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->hour('hour', Map {'defaultHour' => 8, 'onchange' => 'update(this);'}));

        // 24 hour
        $this->object->use24Hour();

        $this->assertEquals(
            '<select id="hour" name="hour">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->hour('hour'));

        $this->assertEquals(
            '<select id="hour" name="hour" onchange="update(this);">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option selected="selected" value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->hour('hour', Map {'defaultHour' => 18, 'onchange' => 'update(this);'}));

        // with data
        $this->object->use12Hour();

        $_POST['Test']['hour'] = 4;
        Post::initialize($_POST);

        $this->assertEquals(
            '<select id="test-hour" name="Test[hour]">' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option selected="selected" value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->hour('Test.hour', Map {'defaultHour' => 2}));

        $this->object->use24Hour();

        $_POST['Test']['hour24'] = 22;
        Post::initialize($_POST);

        $this->assertEquals(
            '<select id="test-hour24" name="Test[hour24]">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option selected="selected" value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->hour('Test.hour24', Map {'defaultHour' => 2}));
    }

    public function testIsChecked() {
        $this->assertFalse($this->object->isChecked(null, 'foo'));
        $this->assertFalse($this->object->isChecked(false, 'foo'));
        $this->assertFalse($this->object->isChecked('bar', 'foo'));
        $this->assertFalse($this->object->isChecked(['bar'], 'foo'));

        $this->assertTrue($this->object->isChecked(true, 'foo'));
        $this->assertTrue($this->object->isChecked('foo', 'foo'));
        $this->assertTrue($this->object->isChecked(['bar', 'foo'], 'foo'));
    }

    public function testIsSelected() {
        $this->assertFalse($this->object->isSelected(null, 'foo'));
        $this->assertFalse($this->object->isSelected('bar', 'foo'));
        $this->assertFalse($this->object->isSelected(['bar'], 'foo'));

        $this->assertTrue($this->object->isSelected('foo', 'foo'));
        $this->assertTrue($this->object->isSelected(['bar', 'foo'], 'foo'));
    }

    public function testLabel() {
        $this->assertEquals('<label for="label">Title</label>' . PHP_EOL, $this->object->label('label', 'Title'));

        // escaping
        $this->assertEquals('<label for="label">Title &quot;with&quot; quotes</label>' . PHP_EOL, $this->object->label('label', 'Title "with" quotes'));

        // escaping
        $this->assertEquals('<label class="class-name" for="label-nested-input">Title</label>' . PHP_EOL, $this->object->label('label.nested.input', 'Title', Map {'class' => 'class-name'}));

        // nested input
        $this->assertEquals('<label for="label"><input id="label" name="label" type="checkbox" value="1">' . PHP_EOL . '</label>' . PHP_EOL, $this->object->label('label', $this->object->checkbox('label'), Map {'escape' => false}));
    }

    public function testMeridiem() {
        $this->assertEquals(
            '<select id="meridiem" name="meridiem">' . PHP_EOL .
            '<option value="am">AM</option>' . PHP_EOL .
            '<option value="pm">PM</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->meridiem('meridiem'));

        $this->assertEquals(
            '<select id="meridiem" name="meridiem">' . PHP_EOL .
            '<option value="am">AM</option>' . PHP_EOL .
            '<option selected="selected" value="pm">PM</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->meridiem('meridiem', Map {'defaultMeridiem' => 'pm'}));

        // with data
        $_POST['Test']['meridiem'] = 'am';
        Post::initialize($_POST);

        $this->assertEquals(
            '<select id="test-meridiem" name="Test[meridiem]">' . PHP_EOL .
            '<option selected="selected" value="am">AM</option>' . PHP_EOL .
            '<option value="pm">PM</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->meridiem('Test.meridiem', Map {'defaultMeridiem' => 'pm'}));
    }

    public function testMinute() {
        $this->assertEquals(
            '<select id="minute" name="minute">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '<option value="32">32</option>' . PHP_EOL .
            '<option value="33">33</option>' . PHP_EOL .
            '<option value="34">34</option>' . PHP_EOL .
            '<option value="35">35</option>' . PHP_EOL .
            '<option value="36">36</option>' . PHP_EOL .
            '<option value="37">37</option>' . PHP_EOL .
            '<option value="38">38</option>' . PHP_EOL .
            '<option value="39">39</option>' . PHP_EOL .
            '<option value="40">40</option>' . PHP_EOL .
            '<option value="41">41</option>' . PHP_EOL .
            '<option value="42">42</option>' . PHP_EOL .
            '<option value="43">43</option>' . PHP_EOL .
            '<option value="44">44</option>' . PHP_EOL .
            '<option value="45">45</option>' . PHP_EOL .
            '<option value="46">46</option>' . PHP_EOL .
            '<option value="47">47</option>' . PHP_EOL .
            '<option value="48">48</option>' . PHP_EOL .
            '<option value="49">49</option>' . PHP_EOL .
            '<option value="50">50</option>' . PHP_EOL .
            '<option value="51">51</option>' . PHP_EOL .
            '<option value="52">52</option>' . PHP_EOL .
            '<option value="53">53</option>' . PHP_EOL .
            '<option value="54">54</option>' . PHP_EOL .
            '<option value="55">55</option>' . PHP_EOL .
            '<option value="56">56</option>' . PHP_EOL .
            '<option value="57">57</option>' . PHP_EOL .
            '<option value="58">58</option>' . PHP_EOL .
            '<option value="59">59</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->minute('minute'));

        // default
        $this->assertEquals(
            '<select id="minute" name="minute">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '<option value="32">32</option>' . PHP_EOL .
            '<option value="33">33</option>' . PHP_EOL .
            '<option value="34">34</option>' . PHP_EOL .
            '<option value="35">35</option>' . PHP_EOL .
            '<option value="36">36</option>' . PHP_EOL .
            '<option value="37">37</option>' . PHP_EOL .
            '<option selected="selected" value="38">38</option>' . PHP_EOL .
            '<option value="39">39</option>' . PHP_EOL .
            '<option value="40">40</option>' . PHP_EOL .
            '<option value="41">41</option>' . PHP_EOL .
            '<option value="42">42</option>' . PHP_EOL .
            '<option value="43">43</option>' . PHP_EOL .
            '<option value="44">44</option>' . PHP_EOL .
            '<option value="45">45</option>' . PHP_EOL .
            '<option value="46">46</option>' . PHP_EOL .
            '<option value="47">47</option>' . PHP_EOL .
            '<option value="48">48</option>' . PHP_EOL .
            '<option value="49">49</option>' . PHP_EOL .
            '<option value="50">50</option>' . PHP_EOL .
            '<option value="51">51</option>' . PHP_EOL .
            '<option value="52">52</option>' . PHP_EOL .
            '<option value="53">53</option>' . PHP_EOL .
            '<option value="54">54</option>' . PHP_EOL .
            '<option value="55">55</option>' . PHP_EOL .
            '<option value="56">56</option>' . PHP_EOL .
            '<option value="57">57</option>' . PHP_EOL .
            '<option value="58">58</option>' . PHP_EOL .
            '<option value="59">59</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->minute('minute', Map {'defaultMinute' => 38}));

        // with data
        $_POST['Test']['minute'] = 19;
        Post::initialize($_POST);

        $this->assertEquals(
            '<select id="test-minute" name="Test[minute]">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option selected="selected" value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '<option value="32">32</option>' . PHP_EOL .
            '<option value="33">33</option>' . PHP_EOL .
            '<option value="34">34</option>' . PHP_EOL .
            '<option value="35">35</option>' . PHP_EOL .
            '<option value="36">36</option>' . PHP_EOL .
            '<option value="37">37</option>' . PHP_EOL .
            '<option value="38">38</option>' . PHP_EOL .
            '<option value="39">39</option>' . PHP_EOL .
            '<option value="40">40</option>' . PHP_EOL .
            '<option value="41">41</option>' . PHP_EOL .
            '<option value="42">42</option>' . PHP_EOL .
            '<option value="43">43</option>' . PHP_EOL .
            '<option value="44">44</option>' . PHP_EOL .
            '<option value="45">45</option>' . PHP_EOL .
            '<option value="46">46</option>' . PHP_EOL .
            '<option value="47">47</option>' . PHP_EOL .
            '<option value="48">48</option>' . PHP_EOL .
            '<option value="49">49</option>' . PHP_EOL .
            '<option value="50">50</option>' . PHP_EOL .
            '<option value="51">51</option>' . PHP_EOL .
            '<option value="52">52</option>' . PHP_EOL .
            '<option value="53">53</option>' . PHP_EOL .
            '<option value="54">54</option>' . PHP_EOL .
            '<option value="55">55</option>' . PHP_EOL .
            '<option value="56">56</option>' . PHP_EOL .
            '<option value="57">57</option>' . PHP_EOL .
            '<option value="58">58</option>' . PHP_EOL .
            '<option value="59">59</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->minute('Test.minute', Map {'defaultMinute' => 38}));
    }

    public function testModel() {
        if (!class_exists('Titon\Model\Model')) {
            $this->markTestSkipped('Test skipped; Please install titon/model via Composer');
        }

        $model = new Model();

        $this->assertEquals('<form accept-charset="UTF-8" action="/" enctype="application/x-www-form-urlencoded" id="model-form-2" method="POST">' . PHP_EOL, $this->object->model($model, '/'));
        $this->assertEquals($model, $this->object->getModel());
    }

    public function testMonth() {
        $this->assertEquals(
            '<select id="month" name="month">' . PHP_EOL .
            '<option value="1">January</option>' . PHP_EOL .
            '<option value="2">February</option>' . PHP_EOL .
            '<option value="3">March</option>' . PHP_EOL .
            '<option value="4">April</option>' . PHP_EOL .
            '<option value="5">May</option>' . PHP_EOL .
            '<option value="6">June</option>' . PHP_EOL .
            '<option value="7">July</option>' . PHP_EOL .
            '<option value="8">August</option>' . PHP_EOL .
            '<option value="9">September</option>' . PHP_EOL .
            '<option value="10">October</option>' . PHP_EOL .
            '<option value="11">November</option>' . PHP_EOL .
            '<option value="12">December</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->month('month'));

        // format + default
        $this->assertEquals(
            '<select id="month" name="month">' . PHP_EOL .
            '<option value="1">Jan</option>' . PHP_EOL .
            '<option value="2">Feb</option>' . PHP_EOL .
            '<option value="3">Mar</option>' . PHP_EOL .
            '<option value="4">Apr</option>' . PHP_EOL .
            '<option value="5">May</option>' . PHP_EOL .
            '<option value="6">Jun</option>' . PHP_EOL .
            '<option value="7">Jul</option>' . PHP_EOL .
            '<option selected="selected" value="8">Aug</option>' . PHP_EOL .
            '<option value="9">Sep</option>' . PHP_EOL .
            '<option value="10">Oct</option>' . PHP_EOL .
            '<option value="11">Nov</option>' . PHP_EOL .
            '<option value="12">Dec</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->month('month', Map {'defaultMonth' => 8, 'monthFormat' => 'M'}));

        // with data
        $_POST['Test']['month'] = 2;
        Post::initialize($_POST);

        $this->assertEquals(
            '<select id="test-month" name="Test[month]">' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option selected="selected" value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->month('Test.month', Map {'defaultMonth' => 8, 'monthFormat' => 'm'}));
    }

    public function testOpen() {
        $this->assertEquals('<form accept-charset="UTF-8" action="/" enctype="application/x-www-form-urlencoded" id="form-2" method="POST">' . PHP_EOL, $this->object->open('/'));

        // file
        $this->assertEquals('<form accept-charset="UTF-8" action="/" enctype="multipart/form-data" id="form-3" method="POST">' . PHP_EOL, $this->object->open('/', Map {'type' => 'file'}));

        // action
        $this->assertEquals('<form accept-charset="UTF-8" action="/search" enctype="application/x-www-form-urlencoded" id="form-4" method="GET">' . PHP_EOL, $this->object->open('/search', Map {'method' => 'get'}));
    }

    public function testPassword() {
        $this->assertEquals('<input id="password" name="password" type="password">' . PHP_EOL, $this->object->password('password'));

        // no value
        $this->assertEquals('<input id="password" name="password" type="password">' . PHP_EOL, $this->object->password('password', Map {'value' => 'none'}));

        // with data
        $_POST['Test']['password'] = 'nasd7ads';
        Post::initialize($_POST);

        $this->assertEquals('<input id="test-password" name="Test[password]" type="password">' . PHP_EOL, $this->object->password('Test.password'));
    }

    public function testPrepareAttributes() {
        $this->assertEquals(Map {
            'name' => 'foo[bar]',
            'multiple' => 'multiple',
            'disabled' => 'disabled',
            'id' => 'foo-bar'
        }, $this->object->prepareAttributes(Map {'name' => 'foobar'}, Map {
            'name' => 'foo.bar',
            'multiple' => true,
            'readonly' => false,
            'disabled' => 'disabled'
        }));

        $this->assertEquals(Map {
            'name' => 'foo[bar]',
            'id' => 'custom-id',
            'empty' => true
        }, $this->object->prepareAttributes(Map {'name' => 'foo.bar', 'id' => 'custom-id'}, Map {'empty' => true}));
    }

    public function testRadio() {
        $this->assertEquals('<input id="radio" name="radio" type="radio" value="1">' . PHP_EOL, $this->object->radio('radio', '1'));

        // tiered depth
        $this->assertEquals('<input id="radio-box" name="radio[box]" type="radio" value="1">' . PHP_EOL, $this->object->radio('radio.box', '1'));

        // attributes
        $this->assertEquals('<input class="class-name" id="custom-id" name="radio" type="radio" value="1">' . PHP_EOL, $this->object->radio('radio', '1', Map {
            'class' => 'class-name',
            'id' => 'custom-id'
        }));

        // value
        $this->assertEquals('<input id="radio" name="radio" type="radio" value="yes">' . PHP_EOL, $this->object->radio('radio', 'yes'));

        // value with data
        $_POST['Test']['radio'] = 'no';
        Post::initialize($_POST);

        $this->assertEquals('<input id="test-radio" name="Test[radio]" type="radio" value="yes">' . PHP_EOL, $this->object->radio('Test.radio', 'yes'));

        $_POST['Test']['radio'] = 'yes';
        Post::initialize($_POST);

        $this->assertEquals('<input checked="checked" id="test-radio" name="Test[radio]" type="radio" value="yes">' . PHP_EOL, $this->object->radio('Test.radio', 'yes'));
    }

    public function testRadios() {
        $options = Vector {'red', 'blue', 'green'};

        // regular
        $this->assertEquals(Vector {
            '<input id="radio-red" name="radio" type="radio" value="red">' . PHP_EOL,
            '<input id="radio-blue" name="radio" type="radio" value="blue">' . PHP_EOL,
            '<input id="radio-green" name="radio" type="radio" value="green">' . PHP_EOL
        }, $this->object->radios('radio', $options));

        // default
        $this->assertEquals(Vector {
            '<input class="radio" id="radio-red" name="radio" type="radio" value="red">' . PHP_EOL,
            '<input class="radio" id="radio-blue" name="radio" type="radio" value="blue">' . PHP_EOL,
            '<input checked="checked" class="radio" id="radio-green" name="radio" type="radio" value="green">' . PHP_EOL
        }, $this->object->radios('radio', $options, Map {'default' => 'green', 'class' => 'radio'}));

        // with data
        $_POST['Test']['radio'] = 'red';
        Post::initialize($_POST);

        $this->assertEquals(Vector {
            '<input checked="checked" id="test-radio-red" name="Test[radio]" type="radio" value="red">' . PHP_EOL,
            '<input id="test-radio-blue" name="Test[radio]" type="radio" value="blue">' . PHP_EOL,
            '<input id="test-radio-green" name="Test[radio]" type="radio" value="green">' . PHP_EOL
        }, $this->object->radios('Test.radio', $options));
    }

    public function testReset() {
        $this->assertEquals('<button id="form-1-reset" type="reset">Title</button>' . PHP_EOL, $this->object->reset('Title'));

        // escaping
        $this->assertEquals('<button id="form-1-reset" type="reset">Title &quot;with&quot; quotes</button>' . PHP_EOL, $this->object->reset('Title "with" quotes'));

        // attributes
        $this->assertEquals('<button class="reset" id="reset" type="reset">Title</button>' . PHP_EOL, $this->object->reset('Title', Map {'class' => 'reset', 'id' => 'reset'}));
    }

    public function testSecond() {
        $this->assertEquals(
            '<select id="second" name="second">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '<option value="32">32</option>' . PHP_EOL .
            '<option value="33">33</option>' . PHP_EOL .
            '<option value="34">34</option>' . PHP_EOL .
            '<option value="35">35</option>' . PHP_EOL .
            '<option value="36">36</option>' . PHP_EOL .
            '<option value="37">37</option>' . PHP_EOL .
            '<option value="38">38</option>' . PHP_EOL .
            '<option value="39">39</option>' . PHP_EOL .
            '<option value="40">40</option>' . PHP_EOL .
            '<option value="41">41</option>' . PHP_EOL .
            '<option value="42">42</option>' . PHP_EOL .
            '<option value="43">43</option>' . PHP_EOL .
            '<option value="44">44</option>' . PHP_EOL .
            '<option value="45">45</option>' . PHP_EOL .
            '<option value="46">46</option>' . PHP_EOL .
            '<option value="47">47</option>' . PHP_EOL .
            '<option value="48">48</option>' . PHP_EOL .
            '<option value="49">49</option>' . PHP_EOL .
            '<option value="50">50</option>' . PHP_EOL .
            '<option value="51">51</option>' . PHP_EOL .
            '<option value="52">52</option>' . PHP_EOL .
            '<option value="53">53</option>' . PHP_EOL .
            '<option value="54">54</option>' . PHP_EOL .
            '<option value="55">55</option>' . PHP_EOL .
            '<option value="56">56</option>' . PHP_EOL .
            '<option value="57">57</option>' . PHP_EOL .
            '<option value="58">58</option>' . PHP_EOL .
            '<option value="59">59</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->second('second'));

        // default
        $this->assertEquals(
            '<select id="second" name="second">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '<option value="32">32</option>' . PHP_EOL .
            '<option value="33">33</option>' . PHP_EOL .
            '<option value="34">34</option>' . PHP_EOL .
            '<option value="35">35</option>' . PHP_EOL .
            '<option value="36">36</option>' . PHP_EOL .
            '<option value="37">37</option>' . PHP_EOL .
            '<option value="38">38</option>' . PHP_EOL .
            '<option value="39">39</option>' . PHP_EOL .
            '<option value="40">40</option>' . PHP_EOL .
            '<option value="41">41</option>' . PHP_EOL .
            '<option value="42">42</option>' . PHP_EOL .
            '<option value="43">43</option>' . PHP_EOL .
            '<option value="44">44</option>' . PHP_EOL .
            '<option value="45">45</option>' . PHP_EOL .
            '<option value="46">46</option>' . PHP_EOL .
            '<option value="47">47</option>' . PHP_EOL .
            '<option value="48">48</option>' . PHP_EOL .
            '<option value="49">49</option>' . PHP_EOL .
            '<option value="50">50</option>' . PHP_EOL .
            '<option value="51">51</option>' . PHP_EOL .
            '<option value="52">52</option>' . PHP_EOL .
            '<option value="53">53</option>' . PHP_EOL .
            '<option value="54">54</option>' . PHP_EOL .
            '<option value="55">55</option>' . PHP_EOL .
            '<option value="56">56</option>' . PHP_EOL .
            '<option value="57">57</option>' . PHP_EOL .
            '<option selected="selected" value="58">58</option>' . PHP_EOL .
            '<option value="59">59</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->second('second', Map {'defaultSecond' => 58}));

        // with data
        $_POST['Test']['second'] = 40;
        Post::initialize($_POST);

        $this->assertEquals(
            '<select id="test-second" name="Test[second]">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '<option value="32">32</option>' . PHP_EOL .
            '<option value="33">33</option>' . PHP_EOL .
            '<option value="34">34</option>' . PHP_EOL .
            '<option value="35">35</option>' . PHP_EOL .
            '<option value="36">36</option>' . PHP_EOL .
            '<option value="37">37</option>' . PHP_EOL .
            '<option value="38">38</option>' . PHP_EOL .
            '<option value="39">39</option>' . PHP_EOL .
            '<option selected="selected" value="40">40</option>' . PHP_EOL .
            '<option value="41">41</option>' . PHP_EOL .
            '<option value="42">42</option>' . PHP_EOL .
            '<option value="43">43</option>' . PHP_EOL .
            '<option value="44">44</option>' . PHP_EOL .
            '<option value="45">45</option>' . PHP_EOL .
            '<option value="46">46</option>' . PHP_EOL .
            '<option value="47">47</option>' . PHP_EOL .
            '<option value="48">48</option>' . PHP_EOL .
            '<option value="49">49</option>' . PHP_EOL .
            '<option value="50">50</option>' . PHP_EOL .
            '<option value="51">51</option>' . PHP_EOL .
            '<option value="52">52</option>' . PHP_EOL .
            '<option value="53">53</option>' . PHP_EOL .
            '<option value="54">54</option>' . PHP_EOL .
            '<option value="55">55</option>' . PHP_EOL .
            '<option value="56">56</option>' . PHP_EOL .
            '<option value="57">57</option>' . PHP_EOL .
            '<option value="58">58</option>' . PHP_EOL .
            '<option value="59">59</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->second('Test.second', Map {'defaultSecond' => 58}));
    }

    public function testSelect() {
        $options = Map {'warrior' => 'Warrior', 'ranger' => 'Ranger', 'mage' => 'Mage'};

        $this->assertEquals(
            '<select id="select" name="select">' . PHP_EOL .
            '<option value="warrior">Warrior</option>' . PHP_EOL .
            '<option value="ranger">Ranger</option>' . PHP_EOL .
            '<option value="mage">Mage</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->select('select', $options));

        // empty
        $this->assertEquals(
            '<select id="select" name="select">' . PHP_EOL .
            '<option value=""></option>' . PHP_EOL .
            '<option value="warrior">Warrior</option>' . PHP_EOL .
            '<option value="ranger">Ranger</option>' . PHP_EOL .
            '<option value="mage">Mage</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->select('select', $options, Map {'empty' => true}));

        $this->assertEquals(
            '<select id="select" name="select">' . PHP_EOL .
            '<option value="">-- None --</option>' . PHP_EOL .
            '<option value="warrior">Warrior</option>' . PHP_EOL .
            '<option value="ranger">Ranger</option>' . PHP_EOL .
            '<option value="mage">Mage</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->select('select', $options, Map {'empty' => '-- None --'}));

        // default
        $this->assertEquals(
            '<select id="select" name="select">' . PHP_EOL .
            '<option value="warrior">Warrior</option>' . PHP_EOL .
            '<option selected="selected" value="ranger">Ranger</option>' . PHP_EOL .
            '<option value="mage">Mage</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->select('select', $options, Map {'default' => 'ranger'}));

        // multiple
        $this->assertEquals(
            '<select id="select" multiple="multiple" name="select">' . PHP_EOL .
            '<option value="warrior">Warrior</option>' . PHP_EOL .
            '<option selected="selected" value="ranger">Ranger</option>' . PHP_EOL .
            '<option selected="selected" value="mage">Mage</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->select('select', $options, Map {'default' => ['ranger', 'mage'], 'multiple' => true}));

        // optgroup
        $optgroup = Map {
            'Weapons' => Map {
                'axe' => 'Axe',
                'sword' => 'Sword',
                'bow' => 'Bow',
                'staff' => 'Staff'
            },
            'Classes' => $options
        };

        $this->assertEquals(
            '<select id="selectgroup" name="select_group">' . PHP_EOL .
            '<optgroup label="Weapons">' . PHP_EOL .
            '<option value="axe">Axe</option>' . PHP_EOL .
            '<option value="sword">Sword</option>' . PHP_EOL .
            '<option value="bow">Bow</option>' . PHP_EOL .
            '<option value="staff">Staff</option>' . PHP_EOL .
            '</optgroup>' . PHP_EOL .
            '<optgroup label="Classes">' . PHP_EOL .
            '<option value="warrior">Warrior</option>' . PHP_EOL .
            '<option value="ranger">Ranger</option>' . PHP_EOL .
            '<option value="mage">Mage</option>' . PHP_EOL .
            '</optgroup>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->select('select_group', $optgroup));

        $this->assertEquals(
            '<select id="selectgroup" name="select_group">' . PHP_EOL .
            '<optgroup label="Weapons">' . PHP_EOL .
            '<option value="axe">Axe</option>' . PHP_EOL .
            '<option value="sword">Sword</option>' . PHP_EOL .
            '<option selected="selected" value="bow">Bow</option>' . PHP_EOL .
            '<option value="staff">Staff</option>' . PHP_EOL .
            '</optgroup>' . PHP_EOL .
            '<optgroup label="Classes">' . PHP_EOL .
            '<option value="warrior">Warrior</option>' . PHP_EOL .
            '<option value="ranger">Ranger</option>' . PHP_EOL .
            '<option value="mage">Mage</option>' . PHP_EOL .
            '</optgroup>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->select('select_group', $optgroup, Map {'default' => 'bow'}));

        $this->assertEquals(
            '<select id="selectgroup" multiple="multiple" name="select_group">' . PHP_EOL .
            '<optgroup label="Weapons">' . PHP_EOL .
            '<option selected="selected" value="axe">Axe</option>' . PHP_EOL .
            '<option value="sword">Sword</option>' . PHP_EOL .
            '<option selected="selected" value="bow">Bow</option>' . PHP_EOL .
            '<option value="staff">Staff</option>' . PHP_EOL .
            '</optgroup>' . PHP_EOL .
            '<optgroup label="Classes">' . PHP_EOL .
            '<option value="warrior">Warrior</option>' . PHP_EOL .
            '<option value="ranger">Ranger</option>' . PHP_EOL .
            '<option selected="selected" value="mage">Mage</option>' . PHP_EOL .
            '</optgroup>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->select('select_group', $optgroup, Map {'default' => ['bow', 'axe', 'mage'], 'multiple' => 'multiple'}));

        // with data
        $_POST['Test']['select'] = 'mage';
        Post::initialize($_POST);

        $this->assertEquals(
            '<select id="test-select" name="Test[select]">' . PHP_EOL .
            '<option value="warrior">Warrior</option>' . PHP_EOL .
            '<option value="ranger">Ranger</option>' . PHP_EOL .
            '<option selected="selected" value="mage">Mage</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->select('Test.select', $options));

        $_POST['Test']['select_group'] = ['sword', 'warrior'];
        Post::initialize($_POST);

        $this->assertEquals(
            '<select id="test-selectgroup" multiple="multiple" name="Test[select_group]">' . PHP_EOL .
            '<optgroup label="Weapons">' . PHP_EOL .
            '<option value="axe">Axe</option>' . PHP_EOL .
            '<option selected="selected" value="sword">Sword</option>' . PHP_EOL .
            '<option value="bow">Bow</option>' . PHP_EOL .
            '<option value="staff">Staff</option>' . PHP_EOL .
            '</optgroup>' . PHP_EOL .
            '<optgroup label="Classes">' . PHP_EOL .
            '<option selected="selected" value="warrior">Warrior</option>' . PHP_EOL .
            '<option value="ranger">Ranger</option>' . PHP_EOL .
            '<option value="mage">Mage</option>' . PHP_EOL .
            '</optgroup>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->select('Test.select_group', $optgroup, Map {'multiple' => true}));
    }

    public function testSubmit() {
        $this->assertEquals('<button id="form-1-submit" type="submit">Title</button>' . PHP_EOL, $this->object->submit('Title'));

        // escaping
        $this->assertEquals('<button id="form-1-submit" type="submit">Title &quot;with&quot; quotes</button>' . PHP_EOL, $this->object->submit('Title "with" quotes'));

        // attributes
        $this->assertEquals('<button class="reset" id="submit" type="submit">Title</button>' . PHP_EOL, $this->object->submit('Title', Map {'class' => 'reset', 'id' => 'submit'}));
    }

    public function testText() {
        $this->assertEquals('<input id="text" name="text" type="text" value="">' . PHP_EOL, $this->object->text('text'));
        $this->assertEquals('<input class="input" id="text" name="text" placeholder="Testing &quot;quotes&quot; placeholder" readonly="readonly" type="text" value="">' . PHP_EOL, $this->object->text('text', '', Map {
            'placeholder' => 'Testing "quotes" placeholder',
            'class' => 'input',
            'readonly' => true
        }));

        // with data
        $this->assertEquals('<input id="test-text" name="Test[text]" type="text" value="foo">' . PHP_EOL, $this->object->text('Test.text', 'foo'));

        $_POST['Test']['text'] = 'bar';
        Post::initialize($_POST);

        $this->assertEquals('<input id="test-text" name="Test[text]" type="text" value="bar">' . PHP_EOL, $this->object->text('Test.text', 'foo'));
    }

    public function testTextarea() {
        $this->assertEquals('<textarea cols="25" id="textarea" name="textarea" rows="5"></textarea>' . PHP_EOL, $this->object->textarea('textarea'));
        $this->assertEquals('<textarea class="input" cols="50" disabled="disabled" id="textarea" name="textarea" rows="10"></textarea>' . PHP_EOL, $this->object->textarea('textarea', '', Map {
            'class' => 'input',
            'rows' => 10,
            'cols' => 50,
            'disabled' => true
        }));

        // with data
        $this->assertEquals('<textarea cols="25" id="test-textarea" name="Test[textarea]" rows="5">foo</textarea>' . PHP_EOL, $this->object->textarea('Test.textarea', 'foo'));

        $_POST['Test']['textarea'] = 'bar';
        Post::initialize($_POST);

        $this->assertEquals('<textarea cols="25" id="test-textarea" name="Test[textarea]" rows="5">bar</textarea>' . PHP_EOL, $this->object->textarea('Test.textarea', 'foo'));
    }

    public function testTime() {
        $this->object->use12Hour();

        $this->assertEquals('<select id="created-hour" name="created[hour]">' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            ':<select id="created-minute" name="created[minute]">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '<option value="32">32</option>' . PHP_EOL .
            '<option value="33">33</option>' . PHP_EOL .
            '<option value="34">34</option>' . PHP_EOL .
            '<option value="35">35</option>' . PHP_EOL .
            '<option value="36">36</option>' . PHP_EOL .
            '<option value="37">37</option>' . PHP_EOL .
            '<option value="38">38</option>' . PHP_EOL .
            '<option value="39">39</option>' . PHP_EOL .
            '<option value="40">40</option>' . PHP_EOL .
            '<option value="41">41</option>' . PHP_EOL .
            '<option value="42">42</option>' . PHP_EOL .
            '<option value="43">43</option>' . PHP_EOL .
            '<option value="44">44</option>' . PHP_EOL .
            '<option value="45">45</option>' . PHP_EOL .
            '<option value="46">46</option>' . PHP_EOL .
            '<option value="47">47</option>' . PHP_EOL .
            '<option value="48">48</option>' . PHP_EOL .
            '<option value="49">49</option>' . PHP_EOL .
            '<option value="50">50</option>' . PHP_EOL .
            '<option value="51">51</option>' . PHP_EOL .
            '<option value="52">52</option>' . PHP_EOL .
            '<option value="53">53</option>' . PHP_EOL .
            '<option value="54">54</option>' . PHP_EOL .
            '<option value="55">55</option>' . PHP_EOL .
            '<option value="56">56</option>' . PHP_EOL .
            '<option value="57">57</option>' . PHP_EOL .
            '<option value="58">58</option>' . PHP_EOL .
            '<option value="59">59</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            ':<select id="created-second" name="created[second]">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '<option value="32">32</option>' . PHP_EOL .
            '<option value="33">33</option>' . PHP_EOL .
            '<option value="34">34</option>' . PHP_EOL .
            '<option value="35">35</option>' . PHP_EOL .
            '<option value="36">36</option>' . PHP_EOL .
            '<option value="37">37</option>' . PHP_EOL .
            '<option value="38">38</option>' . PHP_EOL .
            '<option value="39">39</option>' . PHP_EOL .
            '<option value="40">40</option>' . PHP_EOL .
            '<option value="41">41</option>' . PHP_EOL .
            '<option value="42">42</option>' . PHP_EOL .
            '<option value="43">43</option>' . PHP_EOL .
            '<option value="44">44</option>' . PHP_EOL .
            '<option value="45">45</option>' . PHP_EOL .
            '<option value="46">46</option>' . PHP_EOL .
            '<option value="47">47</option>' . PHP_EOL .
            '<option value="48">48</option>' . PHP_EOL .
            '<option value="49">49</option>' . PHP_EOL .
            '<option value="50">50</option>' . PHP_EOL .
            '<option value="51">51</option>' . PHP_EOL .
            '<option value="52">52</option>' . PHP_EOL .
            '<option value="53">53</option>' . PHP_EOL .
            '<option value="54">54</option>' . PHP_EOL .
            '<option value="55">55</option>' . PHP_EOL .
            '<option value="56">56</option>' . PHP_EOL .
            '<option value="57">57</option>' . PHP_EOL .
            '<option value="58">58</option>' . PHP_EOL .
            '<option value="59">59</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            '<select id="created-meridiem" name="created[meridiem]">' . PHP_EOL .
            '<option value="am">AM</option>' . PHP_EOL .
            '<option value="pm">PM</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->time('created'));

        $this->object->use24Hour();

        $this->assertEquals('<select id="created-hour" name="created[hour]">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            ':<select id="created-minute" name="created[minute]">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '<option value="32">32</option>' . PHP_EOL .
            '<option value="33">33</option>' . PHP_EOL .
            '<option value="34">34</option>' . PHP_EOL .
            '<option value="35">35</option>' . PHP_EOL .
            '<option value="36">36</option>' . PHP_EOL .
            '<option value="37">37</option>' . PHP_EOL .
            '<option value="38">38</option>' . PHP_EOL .
            '<option value="39">39</option>' . PHP_EOL .
            '<option value="40">40</option>' . PHP_EOL .
            '<option value="41">41</option>' . PHP_EOL .
            '<option value="42">42</option>' . PHP_EOL .
            '<option value="43">43</option>' . PHP_EOL .
            '<option value="44">44</option>' . PHP_EOL .
            '<option value="45">45</option>' . PHP_EOL .
            '<option value="46">46</option>' . PHP_EOL .
            '<option value="47">47</option>' . PHP_EOL .
            '<option value="48">48</option>' . PHP_EOL .
            '<option value="49">49</option>' . PHP_EOL .
            '<option value="50">50</option>' . PHP_EOL .
            '<option value="51">51</option>' . PHP_EOL .
            '<option value="52">52</option>' . PHP_EOL .
            '<option value="53">53</option>' . PHP_EOL .
            '<option value="54">54</option>' . PHP_EOL .
            '<option value="55">55</option>' . PHP_EOL .
            '<option value="56">56</option>' . PHP_EOL .
            '<option value="57">57</option>' . PHP_EOL .
            '<option value="58">58</option>' . PHP_EOL .
            '<option value="59">59</option>' . PHP_EOL .
            '</select>' . PHP_EOL .
            ':<select id="created-second" name="created[second]">' . PHP_EOL .
            '<option value="0">00</option>' . PHP_EOL .
            '<option value="1">01</option>' . PHP_EOL .
            '<option value="2">02</option>' . PHP_EOL .
            '<option value="3">03</option>' . PHP_EOL .
            '<option value="4">04</option>' . PHP_EOL .
            '<option value="5">05</option>' . PHP_EOL .
            '<option value="6">06</option>' . PHP_EOL .
            '<option value="7">07</option>' . PHP_EOL .
            '<option value="8">08</option>' . PHP_EOL .
            '<option value="9">09</option>' . PHP_EOL .
            '<option value="10">10</option>' . PHP_EOL .
            '<option value="11">11</option>' . PHP_EOL .
            '<option value="12">12</option>' . PHP_EOL .
            '<option value="13">13</option>' . PHP_EOL .
            '<option value="14">14</option>' . PHP_EOL .
            '<option value="15">15</option>' . PHP_EOL .
            '<option value="16">16</option>' . PHP_EOL .
            '<option value="17">17</option>' . PHP_EOL .
            '<option value="18">18</option>' . PHP_EOL .
            '<option value="19">19</option>' . PHP_EOL .
            '<option value="20">20</option>' . PHP_EOL .
            '<option value="21">21</option>' . PHP_EOL .
            '<option value="22">22</option>' . PHP_EOL .
            '<option value="23">23</option>' . PHP_EOL .
            '<option value="24">24</option>' . PHP_EOL .
            '<option value="25">25</option>' . PHP_EOL .
            '<option value="26">26</option>' . PHP_EOL .
            '<option value="27">27</option>' . PHP_EOL .
            '<option value="28">28</option>' . PHP_EOL .
            '<option value="29">29</option>' . PHP_EOL .
            '<option value="30">30</option>' . PHP_EOL .
            '<option value="31">31</option>' . PHP_EOL .
            '<option value="32">32</option>' . PHP_EOL .
            '<option value="33">33</option>' . PHP_EOL .
            '<option value="34">34</option>' . PHP_EOL .
            '<option value="35">35</option>' . PHP_EOL .
            '<option value="36">36</option>' . PHP_EOL .
            '<option value="37">37</option>' . PHP_EOL .
            '<option value="38">38</option>' . PHP_EOL .
            '<option value="39">39</option>' . PHP_EOL .
            '<option value="40">40</option>' . PHP_EOL .
            '<option value="41">41</option>' . PHP_EOL .
            '<option value="42">42</option>' . PHP_EOL .
            '<option value="43">43</option>' . PHP_EOL .
            '<option value="44">44</option>' . PHP_EOL .
            '<option value="45">45</option>' . PHP_EOL .
            '<option value="46">46</option>' . PHP_EOL .
            '<option value="47">47</option>' . PHP_EOL .
            '<option value="48">48</option>' . PHP_EOL .
            '<option value="49">49</option>' . PHP_EOL .
            '<option value="50">50</option>' . PHP_EOL .
            '<option value="51">51</option>' . PHP_EOL .
            '<option value="52">52</option>' . PHP_EOL .
            '<option value="53">53</option>' . PHP_EOL .
            '<option value="54">54</option>' . PHP_EOL .
            '<option value="55">55</option>' . PHP_EOL .
            '<option value="56">56</option>' . PHP_EOL .
            '<option value="57">57</option>' . PHP_EOL .
            '<option value="58">58</option>' . PHP_EOL .
            '<option value="59">59</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->time('created'));
    }

    public function testToken() {
        $this->assertEquals('<input id="token" name="_token" type="hidden" value="AJW9120SJ">' . PHP_EOL, $this->object->token('AJW9120SJ'));
    }

    public function testUrl() {
        $this->assertEquals('<input id="url" name="url" type="url" value="">' . PHP_EOL, $this->object->url('url'));

        // with data
        $this->assertEquals('<input id="test-url" name="Test[url]" type="url" value="">' . PHP_EOL, $this->object->url('Test.url'));

        $_POST['Test']['url'] = 'http://domain.com';
        Post::initialize($_POST);

        $this->assertEquals('<input id="test-url" name="Test[url]" type="url" value="http://domain.com">' . PHP_EOL, $this->object->url('Test.url'));
    }

    public function testYear() {
        $this->assertEquals(
            '<select id="year" name="year">' . PHP_EOL .
            '<option value="2005">2005</option>' . PHP_EOL .
            '<option value="2006">2006</option>' . PHP_EOL .
            '<option value="2007">2007</option>' . PHP_EOL .
            '<option value="2008">2008</option>' . PHP_EOL .
            '<option value="2009">2009</option>' . PHP_EOL .
            '<option value="2010">2010</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->year('year', Map {'startYear' => 2005, 'endYear' => 2010}));

        // reverse with default
        $this->assertEquals(
            '<select id="year" name="year">' . PHP_EOL .
            '<option value="2010">2010</option>' . PHP_EOL .
            '<option value="2009">2009</option>' . PHP_EOL .
            '<option value="2008">2008</option>' . PHP_EOL .
            '<option selected="selected" value="2007">2007</option>' . PHP_EOL .
            '<option value="2006">2006</option>' . PHP_EOL .
            '<option value="2005">2005</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->year('year', Map {'startYear' => 2005, 'endYear' => 2010, 'reverseYear' => true, 'defaultYear' => 2007}));

        // reverse with format
        $this->assertEquals(
            '<select id="year" name="year">' . PHP_EOL .
            '<option value="2010">10</option>' . PHP_EOL .
            '<option value="2009">09</option>' . PHP_EOL .
            '<option value="2008">08</option>' . PHP_EOL .
            '<option value="2007">07</option>' . PHP_EOL .
            '<option value="2006">06</option>' . PHP_EOL .
            '<option value="2005">05</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->year('year', Map {'startYear' => 2005, 'endYear' => 2010, 'reverseYear' => true, 'yearFormat' => 'y'}));

        // with data
        $_POST['Test']['year'] = 2005;
        Post::initialize($_POST);

        $this->assertEquals(
            '<select id="test-year" name="Test[year]">' . PHP_EOL .
            '<option value="2010">10</option>' . PHP_EOL .
            '<option value="2009">09</option>' . PHP_EOL .
            '<option value="2008">08</option>' . PHP_EOL .
            '<option value="2007">07</option>' . PHP_EOL .
            '<option value="2006">06</option>' . PHP_EOL .
            '<option selected="selected" value="2005">05</option>' . PHP_EOL .
            '</select>' . PHP_EOL
        , $this->object->year('Test.year', Map {'startYear' => 2005, 'endYear' => 2010, 'reverseYear' => true, 'defaultYear' => 2007, 'yearFormat' => 'y'}));
    }

}