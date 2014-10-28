<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Titon\Model\Model;
use Titon\Utility\Config;
use Titon\Utility\Inflector;
use Titon\Utility\Col;
use Titon\Utility\Path;
use Titon\Utility\State\Files;
use Titon\Utility\State\Get;
use Titon\Utility\State\Post;

type SelectOptionMap = Map<string, string>;
type TimeFormatMap = Map<string, string>;

/**
 * The FormHelper is used for HTML form creation. Data is passed to the associated input fields
 * if a value is present with the request ($_POST, $_GET and $_FILES) or a model.
 *
 * @package Titon\View\Helper
 */
class FormHelper extends AbstractHelper {

    /**
     * Whether to render in 24-hour or 12-hour format.
     *
     * @type bool
     */
    protected bool $_24hour = true;

    /**
     * Mapping of datetime formats.
     *
     * @type \Titon\View\Helper\TimeFormatMap {
     *      @type string $dayFormat     Format to display days in
     *      @type string $monthFormat   Format to display months in
     *      @type string $yearFormat    Format to display years in
     *      @type string $24hour        Display all times in 24 hour formats
     * }
     */
    protected TimeFormatMap $_formats = Map {
        'dayFormat' => 'j',
        'monthFormat' => 'F',
        'yearFormat' => 'Y'
    };

    /**
     * Number of forms generated for the current request.
     *
     * @type int
     */
    protected int $_forms = 0;

    /**
     * The model currently being used to generate a form.
     *
     * @type \Titon\Model\Model
     */
    protected ?Model $_model;

    /**
     * A list of all HTML tags used within the current helper.
     *
     * @type \Titon\View\Helper\TagMap
     */
    protected TagMap $_tags = Map {
        'input'             => '<input{attr}>',
        'textarea'          => '<textarea{attr}>{body}</textarea>',
        'label'             => '<label{attr}>{body}</label>',
        'select'            => '<select{attr}>{body}</select>',
        'option'            => '<option{attr}>{body}</option>',
        'optgroup'          => '<optgroup{attr}>{body}</optgroup>',
        'button'            => '<button{attr}>{body}</button>',
        'legend'            => '<legend>{body}</legend>',
        'form.open'         => '<form{attr}>',
        'form.close'        => '</form>',
        'fieldset.open'     => '<fieldset>',
        'fieldset.close'    => '</fieldset>'
    };

    /**
     * Parses an array of attributes to the HTML equivalent.
     *
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @param Vector<string> $remove
     * @return string
     */
    public function attributes(AttributeMap $attributes, Vector<string> $remove = Vector {}): string {
        $remove->addAll(Vector {
            'defaultDay', 'dayFormat', 'defaultHour', 'defaultMeridiem', 'defaultSecond',
            'defaultMinute', 'defaultMonth', 'monthFormat', 'options', 'default',
            'defaultYear', 'yearFormat', 'reverseYear', 'startYear', 'endYear'
        });

        return parent::attributes($attributes, $remove);
    }

    /**
     * Create a binary checkbox that accepts a 0 or 1 value.
     * Uses a hidden input field as a fallback since checkbox values aren't sent in the request when not checked.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function binary(string $name, AttributeMap $attributes = Map {}): string {

        // Don't use input() since the `value` attribute will change
        $hidden = $this->tag('input', Map {
            'attr' => $this->attributes(Map {
                'value' => '0',
                'type' => 'hidden',
                'name' => $this->formatName($name),
                'id' => $this->formatID($name . '.hidden')
            })
        });

        return $hidden . $this->checkbox($name, '1', $attributes);
    }

    /**
     * Create a single checkbox element.
     *
     * @param string $name
     * @param string $value
     * @param \Titon\View\Helper\AttributeMap $attributes {
     *      @type string $default   The checkbox to be selected by default
     * }
     * @return string
     */
    public function checkbox(string $name, string $value = '1', AttributeMap $attributes = Map {}): string {
        $checked = $this->getValue($name, $attributes);

        if ($this->isChecked($checked, $value)) {
            $attributes['checked'] = true;
        }

        return $this->input($name, $value, 'checkbox', $attributes);
    }

    /**
     * Create an array of multiple checkboxes using the option values.
     *
     * @param string $name
     * @param Vector<string> $options
     * @param \Titon\View\Helper\AttributeMap $attributes {
     *      @type string $default   The checkbox to be selected by default
     * }
     * @return Vector<string>
     */
    public function checkboxes(string $name, Vector<string> $options, AttributeMap $attributes = Map {}): Vector<string> {
        $checkboxes = Vector {};
        $id = $this->formatID($name);

        foreach ($options as $option) {
            $attribs = $attributes->toMap(); // Clone it
            $attribs['id'] = $id . '-' . Inflector::slug($option);
            $attribs['name'] = $name . '.[]';

            $checkboxes[] = $this->checkbox($name, $option, $attribs);
        }

        return $checkboxes;
    }

    /**
     * Close a form by outputting the form close tag and resetting the model.
     *
     * @return string
     */
    public function close(): string {
        $this->_model = null;

        return $this->tag('form.close');
    }

    /**
     * Create a select dropdown for a calendar date: month, day, year.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function date(string $name, AttributeMap $attributes = Map {}): string {
        $map = Map {'name' => $name . '.year'};
        $year = $this->year($name . '.year', $map->setAll($attributes));

        $map = Map {'name' => $name . '.month'};
        $month = $this->month($name . '.month', $map->setAll($attributes));

        $map = Map {'name' => $name . '.day'};
        $day = $this->day($name . '.day', $map->setAll($attributes));

        return $month . ' ' . $day . ' ' . $year;
    }

    /**
     * Combine both date() and time() to output all datetime related dropdowns.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function dateTime(string $name, AttributeMap $attributes = Map {}): string {
        return $this->date($name, $attributes) . ' - ' . $this->time($name, $attributes);
    }

    /**
     * Create a select dropdown for calendar days, with a range of 1-31.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes {
     *      @type string $dayFormat How the day should be formatted
     *      @type int $defaultDay   The default selected day
     * }
     * @return string
     */
    public function day(string $name, AttributeMap $attributes = Map {}): string {
        $format = $this->getAttributeValue('dayFormat', $attributes, 'j');
        $options = Map {};

        for ($i = 1; $i <= 31; ++$i) {
            $v = $i;

            if ($format === 'd') {
                $v = str_pad($v, 2, '0', STR_PAD_LEFT);
            }

            $options[(string) $i] = (string) $v;
        }

        if ($attributes->contains('defaultDay')) {
            $attributes['default'] = $attributes['defaultDay'];
        }

        return $this->select($name, $options, $attributes);
    }

    /**
     * Create an email input field.
     *
     * @param string $name
     * @param string $value
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function email(string $name, string $value = '', AttributeMap $attributes = Map {}): string {
        return $this->input($name, $value, 'email', $attributes);
    }

    /**
     * Create a file upload and browse input field.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function file(string $name, AttributeMap $attributes = Map {}): string {
        return $this->input($name, '', 'file', $attributes);
    }

    /**
     * Format a dot notated path into an HTML/CSS ID name.
     *
     * @param string $name
     * @return string
     */
    public function formatID(string $name): string {
        if ($model = $this->getModel()) {
            $name = Path::className(get_class($model)) . '.' . $name;
        }

        $parts = explode('.', $name);
        $id = Vector {};

        foreach ($parts as $part) {
            if ($part !== '') {
                $id[] = Inflector::slug($part);
            }
        }

        return implode('-', $id);
    }

    /**
     * Format a dot notated path into an input name attribute.
     *
     * @param string $name
     * @return string
     */
    public function formatName(string $name): string {
        $parts = explode('.', $name);
        $path = array_shift($parts);

        foreach ($parts as $part) {
            if ($part === '') {
                continue;
            } else if (strpos($part, '[') === false) {
                $path .= '[' . $part . ']';
            } else {
                $path .= $part;
            }
        }

        return $path;
    }

    /**
     * Get a value from the attributes if it exists, else check the helper properties, and lastly return the default if nothing was found.
     *
     * @param string $key
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @param mixed $default
     * @return mixed
     */
    public function getAttributeValue(string $key, AttributeMap $attributes, mixed $default = null): mixed {
        if ($attributes->contains($key)) {
            return $attributes[$key];

        } else if ($format = $this->getFormat($key)) {
            return $format;
        }

        return $default;
    }

    /**
     * Find and return a default value that has been defined in the attributes.
     *
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return mixed
     */
    public function getDefaultValue(AttributeMap $attributes): mixed {
        return $attributes->get('default');
    }

    /**
     * Return a format by key.
     *
     * @param string $format
     * @return string
     */
    public function getFormat(string $format): string {
        return $this->getFormats()->get($format) ?: '';
    }

    /**
     * Return all formats.
     *
     * @return \Titon\View\Helper\TimeFormatMap
     */
    public function getFormats(): TimeFormatMap {
        return $this->_formats;
    }

    /**
     * Return the current model.
     *
     * @return \Titon\Model\Model
     */
    public function getModel(): ?Model {
        return $this->_model;
    }

    /**
     * Find and return a value from the request.
     *
     * @param string $key
     * @return mixed
     */
    public function getRequestValue(string $key): mixed {
        $data = (new Map([]))
            ->setAll(Get::all())
            ->setAll(Post::all())
            ->setAll(Files::all());

        return Col::extract($data, $key);
    }

    /**
     * Find and return a value from the request, or from a default value if no request is found.
     *
     * @param string $key
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return mixed
     */
    public function getValue(string $key, AttributeMap $attributes = Map {}): mixed {
        $value = $this->getRequestValue($key);
        $model = $this->getModel();

        // Fetch from model attributes directly
        if ($value === null && $model) {
            $value = Col::extract($model->getAttributes(), $key);
        }

        // Fallback to default value
        if ($value === null) {
            $value = $this->getDefaultValue($attributes);
        }

        return $value;
    }

    /**
     * Create a hidden input field.
     *
     * @param string $name
     * @param string $value
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function hidden(string $name, string $value = '', AttributeMap $attributes = Map {}): string {
        return $this->input($name, $value, 'hidden', $attributes);
    }

    /**
     * Create a select dropdown for hours, with a range of 0-23, or 1-12.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes {
     *      @type int $defaultHour  The default selected hour
     * }
     * @return string
     */
    public function hour(string $name, AttributeMap $attributes = Map {}): string {
        $options = Map {};

        if ($this->is24Hour()) {
            $start = 0;
            $end = 23;
        } else {
            $start = 1;
            $end = 12;
        }

        for ($i = $start; $i <= $end; ++$i) {
            $options[(string) $i] = sprintf('%02d', $i);
        }

        if ($attributes->contains('defaultHour')) {
            $attributes['default'] = $attributes['defaultHour'];
        }

        return $this->select($name, $options, $attributes);
    }

    /**
     * Create a standard input field that auto-populates with the correct request value.
     *
     * @param string $name
     * @param string $value
     * @param string $type
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function input(string $name, string $value = '', string $type = 'text', AttributeMap $attributes = Map {}): string {
        $attributes = $this->prepareAttributes(Map {'name' => $name, 'value' => $value, 'type' => $type}, $attributes);

        // Grab current value from the request
        $requestValue = $this->getValue($name, $attributes);

        // Set the value attribute on non-radio/checkbox since those use `checked` attribute
        if ($requestValue !== null && !in_array($type, Vector {'radio', 'checkbox'})) {
            $attributes['value'] = $requestValue;
        }

        // Don't set value attribute for files or password
        if ($type === 'file' || $type === 'password') {
            $attributes->remove('value');
        }

        return $this->tag('input', Map {
            'attr' => $this->attributes($attributes)
        });
    }

    /**
     * Are we using a 12-hour clock?
     *
     * @return bool
     */
    public function is12Hour(): bool {
        return !$this->_24hour;
    }

    /**
     * Are we using a 24-hour clock?
     *
     * @return bool
     */
    public function is24Hour(): bool {
        return $this->_24hour;
    }

    /**
     * Return true if the checkbox/radio should be checked.
     *
     * @param mixed $checked    The checked value from the request, or the fallback default value
     * @param mixed $value      The value for the checkbox
     * @return bool
     */
    public function isChecked(mixed $checked, mixed $value): bool {
        if ($checked === null) {
            return false;
        }

        return (
            // Default checked
            is_bool($checked) && $checked ||
            // Multiple values
            $checked instanceof Traversable && in_array($value, $checked) ||
            // Current value equals the base value
            $checked == $value
        );
    }

    /**
     * Return true if the option in the select should be selected.
     *
     * @param mixed $selected   The selected value from the request, or the fallback default value
     * @param mixed $value      The value for hte option
     * @return bool
     */
    public function isSelected(mixed $selected, mixed $value): bool {
        if ($selected === null) {
            return false;
        }

        return (
            // Multiple values
            $selected instanceof Traversable && in_array($value, $selected) ||
            // Current value equals the base value
            $selected == $value
        );
    }

    /**
     * Create a label form for an input field.
     *
     * @param string $name
     * @param string $title
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function label(string $name, string $title, AttributeMap $attributes = Map {}): string {
        $attributes = (Map {'for' => $this->formatID($name)})->setAll($attributes);

        return $this->tag('label', Map {
            'attr' => $this->attributes($attributes),
            'body' => $this->escape($title, $attributes->get('escape'))
        });
    }

    /**
     * Create a select dropdown for a time meridiem.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes {
     *      @type string $defaultMeridiem   The default selected meridiem
     * }
     * @return string
     */
    public function meridiem(string $name, AttributeMap $attributes = Map {}): string {
        if ($attributes->contains('defaultMeridiem')) {
            $attributes['default'] = $attributes['defaultMeridiem'];
        }

        return $this->select($name, Map {'am' => 'AM', 'pm' => 'PM'}, $attributes);
    }

    /**
     * Create a select dropdown for minutes, with a range of 1-60.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes {
     *      @type int $defaultMinute    The default selected minute
     * }
     * @return string
     */
    public function minute(string $name, AttributeMap $attributes = Map {}): string {
        $options = Map {};

        for ($i = 0; $i <= 59; ++$i) {
            $options[(string) $i] = sprintf('%02d', $i);
        }

        if ($attributes->contains('defaultMinute')) {
            $attributes['default'] = $attributes['defaultMinute'];
        }

        return $this->select($name, $options, $attributes);
    }

    /**
     * Set a model for the form and open the form.
     *
     * @param \Titon\Model\Model $model
     * @param string $action
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function model(Model $model, string $action, AttributeMap $attributes = Map {}): string {
        $this->setModel($model);

        return $this->open($action, $attributes);
    }

    /**
     * Create a select dropdown for calendar months, with a range of 1-12.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes {
     *      @type string $monthFormat   Format the month names in the dropdown
     *      @type int $defaultMonth     The default selected month
     * }
     * @return string
     */
    public function month(string $name, AttributeMap $attributes = Map {}): string {
        $format = $this->getAttributeValue('monthFormat', $attributes, 'F');
        $options = Map {};
        $year = date('Y');

        for ($i = 1; $i <= 12; ++$i) {
            $options[(string) $i] = date($format, mktime(0, 0, 0, $i, 1, $year));
        }

        if ($attributes->contains('defaultMonth')) {
            $attributes['default'] = $attributes['defaultMonth'];
        }

        return $this->select($name, $options, $attributes);
    }

    /**
     * Open a form by outputting the form open tag.
     *
     * @param string $action
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function open(string $action, AttributeMap $attributes = Map {}): string {
        $this->_forms++;

        if ($attributes->contains('type')) {
            if ($attributes['type'] === 'file') {
                $attributes['enctype'] = 'multipart/form-data';
                $attributes['method'] = 'post';
            }

            $attributes->remove('type');
        }

        $attributes = (Map {
            'accept-charset' => Config::encoding(),
            'enctype' => 'application/x-www-form-urlencoded',
            'method' => 'post',
            'action' => $action,
            'id' => $this->formatID('form.' . $this->_forms)
        })->setAll($attributes);

        $attributes['method'] = strtoupper($attributes['method']);

        return $this->tag('form.open', Map {
            'attr' => $this->attributes($attributes)
        });
    }

    /**
     * Create an option element for a select drop down.
     *
     * @param string $label
     * @param string $value
     * @param string $selected
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function option(string $label, string $value, mixed $selected = null, AttributeMap $attributes = Map {}): string {
        $attributes['value'] = $value;

        if ($this->isSelected($selected, $value)) {
            $attributes['selected'] = 'selected';
        }

        return $this->tag('option', Map {
            'attr' => $this->attributes($attributes),
            'body' => $this->escape($label)
        });
    }

    /**
     * Create an optgroup element with a nested list of options.
     *
     * @param string $label
     * @param \Titon\View\Helper\SelectOptionMap $options
     * @param string $selected
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function optionGroup(string $label, SelectOptionMap $options, mixed $selected = null, AttributeMap $attributes = Map {}): string {
        $attributes['label'] = $label;
        $output = PHP_EOL;

        foreach ($options as $value => $option) {
            $output .= $this->option($option, $value, $selected);
        }

        return $this->tag('optgroup', Map {
            'attr' => $this->attributes($attributes),
            'body' => $output
        });
    }

    /**
     * Create a password input field.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function password(string $name, AttributeMap $attributes = Map {}): string {
        return $this->input($name, '', 'password', $attributes);
    }

    /**
     * Parse all the default and required attributes that are used within the input field.
     *
     * @param \Titon\View\Helper\AttributeMap $defaults
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return array
     */
    public function prepareAttributes(AttributeMap $defaults = Map {}, AttributeMap $attributes = Map {}): AttributeMap {
        $attributes = $defaults->setAll($attributes);
        $namePath = $attributes['name'];

        if (!$attributes->contains('id')) {
            $attributes['id'] = $this->formatID($namePath);
        }

        $attributes['name'] = $this->formatName($namePath);

        // Change multiple=true to multiple=multiple
        $remove = Vector {};

        foreach ($attributes as $key => $value) {
            if ($key === 'empty') {
                continue;

            } else if ($value === true) {
                $attributes[$key] = $key;

            } else if ($value === false) {
                $remove[] = $key;
            }
        }

        foreach ($remove as $key) {
            $attributes->remove($key);
        }

        return $attributes;
    }

    /**
     * Create a single radio input element.
     *
     * @param string $name
     * @param string $value
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function radio(string $name, string $value, AttributeMap $attributes = Map {}): string {
        $checked = $this->getValue($name, $attributes);

        if ($this->isChecked($checked, $value)) {
            $attributes['checked'] = true;
        }

        return $this->input($name, $value, 'radio', $attributes);
    }

    /**
     * Create a multiple radio buttons using the array of option values.
     *
     * @param string $name
     * @param Vector<string> $options
     * @param \Titon\View\Helper\AttributeMap $attributes {
     *      @type string default    The radio to be selected by default
     * }
     * @return Vector<string>
     */
    public function radios(string $name, Vector<string> $options, AttributeMap $attributes = Map {}): Vector<string> {
        $radios = Vector {};
        $id = $this->formatID($name);

        foreach ($options as $option) {
            $attribs = $attributes->toMap(); // Clone it
            $attribs['id'] = $id . '-' . Inflector::slug($option);

            $radios[] = $this->radio($name, $option, $attribs);
        }

        return $radios;
    }

    /**
     * Create a form reset button.
     *
     * @param string $title
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function reset(string $title, AttributeMap $attributes = Map {}): string {
        $attributes = (Map {
            'id' => $this->formatID('form.' . $this->_forms . '.reset'),
            'type' => 'reset'
        })->setAll($attributes);

        return $this->tag('button', Map {
            'attr' => $this->attributes($attributes),
            'body' => $this->escape($title, $attributes->get('escape'))
        });
    }

    /**
     * Create a select dropdown for seconds, with a range of 0-59.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes {
     *      @type int $defaultSecond    The default selected second
     * }
     * @return string
     */
    public function second(string $name, AttributeMap $attributes = Map {}): string {
        $options = Map {};

        for ($i = 0; $i <= 59; ++$i) {
            $options[(string) $i] = sprintf('%02d', $i);
        }

        if ($attributes->contains('defaultSecond')) {
            $attributes['default'] = $attributes['defaultSecond'];
        }

        return $this->select($name, $options, $attributes);
    }

    /**
     * Create a select list with values based off an options array.
     *
     * @param string $name
     * @param \Titon\View\Helper\SelectOptionMap $options
     * @param \Titon\View\Helper\AttributeMap $attributes {
     *      @type mixed $default    The option to be selected by default
     *      @type string $empty     Display an empty option at the top of the list
     *      @type bool $multiple    Set to true to allow multiple selection
     * }
     * @return string
     */
    public function select(string $name, SelectOptionMap $options, AttributeMap $attributes = Map {}): string {
        $attributes = $this->prepareAttributes(Map {'name' => $name}, $attributes);
        $output = PHP_EOL;
        $selected = $this->getValue($name, $attributes);

        // Prepend with an empty option
        if ($attributes->contains('empty')) {
            $empty = ($attributes['empty'] === true) ? '' : (string) $attributes['empty'];
            $output .= $this->option($empty, '', $selected);
        }

        // Generate the options
        foreach ($options as $value => $option) {
            if ($option instanceof Map) {
                $output .= $this->optionGroup($value, $option, $selected);
            } else {
                $output .= $this->option($option, $value, $selected);
            }
        }

        return $this->tag('select', Map {
            'attr' => $this->attributes($attributes, Vector {'value', 'empty'}),
            'body' => $output
        });
    }

    /**
     * Set the format for a key.
     *
     * @param string $key
     * @param string $format
     * @return $this
     */
    public function setFormat(string $key, string $format): this {
        $this->_formats[$key] = $format;

        return $this;
    }

    /**
     * Set the model.
     *
     * @param \Titon\Model\Model $model
     * @return $this
     */
    public function setModel(Model $model): this {
        $this->_model = $model;

        return $this;
    }

    /**
     * Create a form submit button.
     *
     * @param string $title
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function submit(string $title, AttributeMap $attributes = Map {}): string {
        $attributes = (Map {
            'id' => $this->formatID('form.' . $this->_forms . '.submit'),
            'type' => 'submit'
        })->setAll($attributes);

        return $this->tag('button', Map {
            'attr' => $this->attributes($attributes),
            'body' => $this->escape($title, $attributes->get('escape'))
        });
    }

    /**
     * Create a basic input text field.
     *
     * @param string $name
     * @param string $value
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function text(string $name, string $value = '', AttributeMap $attributes = Map {}): string {
        return $this->input($name, $value, 'text', $attributes);
    }

    /**
     * Create a textarea field and determine the correct value content.
     *
     * @param string $name
     * @param string $value
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function textarea(string $name, string $value = '', AttributeMap $attributes = Map {}): string {
        $attributes = $this->prepareAttributes(Map {'name' => $name, 'cols' => 25, 'rows' => 5}, $attributes);
        $requestValue = $this->getRequestValue($name);

        if ($requestValue) {
            $value = $requestValue;
        }

        return $this->tag('textarea', Map {
            'attr' => $this->attributes($attributes, Vector {'value'}),
            'body' => $value
        });
    }

    /**
     * Create multiple select dropdowns for hours, minutes, seconds and the meridiem.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function time(string $name, AttributeMap $attributes = Map {}): string {
        $map = Map {'name' => $name . '.hour'};
        $hour = $this->hour($name . '.hour', $map->setAll($attributes));

        $map = Map {'name' => $name . '.minute'};
        $minute = $this->minute($name . '.minute', $map->setAll($attributes));

        $map = Map {'name' => $name . '.second'};
        $second = $this->second($name . '.second', $map->setAll($attributes));

        $output = $hour . ':' . $minute . ':' . $second;

        if ($this->is12Hour()) {
            $map = Map {'name' => $name . '.meridiem'};
            $output .= $this->meridiem($name . '.meridiem', $map->setAll($attributes));
        }

        return $output;
    }

    /**
     * Generate a CSRF protection token field.
     *
     * @param string $value
     * @return string
     */
    public function token(string $value): string {
        return $this->hidden('_token', $value);
    }

    /**
     * Create a url input field.
     *
     * @param string $name
     * @param string $value
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function url(string $name, string $value = '', AttributeMap $attributes = Map {}): string {
        return $this->input($name, $value, 'url', $attributes);
    }

    /**
     * Use a 12-hour clock.
     *
     * @return $this
     */
    public function use12Hour(): this {
        $this->_24hour = false;

        return $this;
    }

    /**
     * Use a 24-hour clock.
     *
     * @return $this
     */
    public function use24Hour(): this {
        $this->_24hour = true;

        return $this;
    }

    /**
     * Create a select dropdown for calendar years, with a user defined range.
     *
     * @param string $name
     * @param \Titon\View\Helper\AttributeMap $attributes {
     *      @type int $startYear        The year to start the range
     *      @type int $endYear          The year to end the range
     *      @type bool $reverseYear     Should the years be in reverse order
     *      @type string $yearFormat    How the year should be formatted
     *      @type int $defaultYear      The default selected year
     * }
     * @return string
     */
    public function year(string $name, AttributeMap $attributes = Map {}): string {
        $month = date('n');
        $year = date('Y');
        $reverse = $this->getAttributeValue('reverseYear', $attributes, false);
        $format = $this->getAttributeValue('yearFormat', $attributes, 'Y');
        $start = (int) $this->getAttributeValue('startYear', $attributes, $year);
        $end = (int) $this->getAttributeValue('endYear', $attributes, ($year + 10));
        $options = Map {};

        for ($i = $start; $i <= $end; ++ $i) {
            $options[(string) $i] = date($format, mktime(0, 0, 0, $month, 1, $i));
        }

        // We must rebuild the map as `array_reverse()` converts the keys to integers
        if ($reverse) {
            $reversed = Map {};

            foreach (array_reverse($options, true) as $key => $value) {
                $reversed[(string) $key] = $value;
            }

            $options = $reversed;
        }

        if ($attributes->contains('defaultYear')) {
            $attributes['default'] = $attributes['defaultYear'];
        }

        return $this->select($name, new Map($options), $attributes);
    }

}