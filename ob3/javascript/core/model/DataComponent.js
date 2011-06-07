/**
 * OB3 DataComponenet
 * @copyright GHC Holdings Ltd 2010, All Rights Reserved
 * @package ob3
 * @subpackage core
 */

OB3.DataComponent = Ext.extend(OB3.Component, {
	
	/**
	 * @see OB3.Component
	 * This is sent in each api request.
	 * @protected {String} className (defaults to "DataComponent")
	 */
	className: 'DataComponent',
	
	/**
	 * @see OB3.Component.components
	 */
	components: [
		
	],
	
	/**
	 * @see OB3.Component.many_components
	 */
	many_components: [
		
	],
	
	/**
	 * An array of fields represented in the api.
	 * eg. [{
	 * 		field: {String} field name
	 * 		type: {String} supported data type
	 * },..]
	 * @protected {Array} fields
	 */
	fields: [
		{field: 'ID', type: 'Int'}
	],
	
	/**
	 * An array of has one relationships represented in the api.
	 * eg. [{
	 * 		field: {String} field name
	 * 		component: {String} component name
	 * 		parent: {Boolean} parent relationship (one per DataComponent)
	 * 		previous: {Boolean} previous relationship (one per DataComponent)
	 * },..]
	 * @protected {Array} has_one
	 */
	has_one: [
		
	],
	
	/**
	 * An array of has one relationships represented in the api.
	 * eg. [{
	 * 		field: {String} field name
	 * 		many_component: {String} many_component name
	 * },..]
	 * @protected {Array} has_one
	 */
	has_many: [
		
	],
	
	/**
	 * True if the DataComponent has been created via the api. This should not be
	 * changed unless you know what you are doing.
	 * @private {Boolean} created (defaults to false)
	 */
	created: false,
	
	/**
	 * True if the DataComponent has been created via the api. This should not be
	 * changed unless you know what you are doing.
	 * @private {Boolean} created (defaults to false)
	 */
	creating: false,
	
	/**
	 * @private {Boolean} join (defaults to false)
	 */
	join: false,
	
	/**
	 * @var {Boolean} allowJoin (defaults to false)
	 */
	allowJoin: true,
	
	/**
	 * True the DataComponent is queued for creation via the api. This should not be
	 * changed unless you know what you are doing.
	 * @private {Boolean} queued (defaults to false)
	 */
	queued: false,
	
	/**
	 * The api to use for read/writes on this DataComponent. Can be changed by 
	 * parent DataComponents and reflects the server side ExtDirect API methods 
	 * and classes. 
	 * @protected {String} api (defaults to "OB3.API.DataComponent")
	 */
	api: 'OB3.API.DataComponent',
	
	/**
	 * The main api provider to use for read/writes, generally this should stay
	 * the same. Only change it if you know what your doing. 
	 * @protected {String} provider (defaults to "OB3.API.provider")
	 */
	provider: 'OB3.API.provider',
	
	constructor: function(config) {
		this.id = 'ob3-datacomponent-'+OB3.util.uniqid();
		/**
		 * Data stored in this component. 
		 * @private {Ext.util.MixedCollection} $record
		 */
		this.$record = new Ext.util.MixedCollection();
		/**
		 * A collection of changed data.
		 * @private {Ext.util.MixedCollection} $changed
		 */
		this.$changed = new Ext.util.MixedCollection();
		/**
		 * An collection listing of changed to be restored.
		 * @private {Array} $restore
		 */
		this.$restore = new Ext.util.MixedCollection();
		/**
		 * The original data stored in this component.
		 * @private {Ext.util.MixedCollection} $original
		 */
		this.$original = new Ext.util.MixedCollection();

		/**
		 * Components indexed by component name.
		 * @private {Ext.util.MixedCollection} $components
		 */
		if(!this.$components) this.$components = new Ext.util.MixedCollection();
		
		/**
		 * Many Components indexed by component name.
		 * @private {Ext.util.MixedCollection} $components
		 */
		if(!this.$manyComponents) this.$manyComponents = new Ext.util.MixedCollection();
		
		//inherit fields and relationships
		var parentClass = this.superclass();
		
		while(parentClass.className !== 'Component') {
			//inherit fields
			var fields = parentClass.fields;
			
			Ext.each(fields, function(field) {
				if(!this.field_defined(field.field)) {
					this.fields.push(field);
				}
			}, this);
			
			//inherit one to one relationships
			var has_ones = parentClass.has_one;
			
			Ext.each(has_ones, function(has_one) {
				if(!this.has_one_defined(has_one.field)) {
					this.has_one.push(has_one);
				}
			}, this);
			
			
			parentClass = parentClass.superclass();
		}
		
		//load fields from config
		if(config && config.values) {
			
			Ext.each(config.values, function(item, index) {
				var field = this.createField(item.field, item.value);
				
				this.$record.add(item.field, field);
				this.$original.add(item.field, field);
				
				if(item.field === 'ID') this.created = true;
			}, this);
			
			delete config.values;
		}
		
		//load fields from config
		if(config && config.has_one) {
			Ext.each(config.has_one, function(item, index) {
				for(var i=0;i<this.has_one.length;i++) {
					var has_one = this.has_one[i];
					
					if(has_one.field == item.field) {
						var cmp = Ext.create({
							xtype: 'OB3.'+item.field,
							components: [{field: this.className, value: this}],
							values: item.values,
							has_one: item.has_one,
							has_many: item.has_many
						});
						
						if(Ext.isObject(cmp)) cmp = cmp.id;
						
						if(this.$components.get(cmp)) {
							this.$components.replace(item.field, cmp);
						}
						else this.$components.add(item.field, cmp);
					}
				}
			}, this);
			
			delete config.has_one;
		}
		
		//load fields from config
		if(config && config.has_many) {
			Ext.each(config.has_many, function(item, index) {
				for(var i=0;i<this.has_many.length;i++) {
					var has_many = this.has_many[i];
					
					if(has_many.field == item.field) {
						var componentSet = this.$manyComponents.get(has_many.field);
						
						if(!componentSet) {
							componentSet = new Ext.util.MixedCollection();
							this.$manyComponents.add(has_many.field, componentSet);
						}
						
						Ext.each(item.components, function(cmp) {
							var cmp = Ext.create({
								xtype: 'OB3.'+cmp.field,
								components: [{field: this.className, value: this}],
								values: cmp.values,
								has_one: cmp.has_one,
								has_many: cmp.has_many
							});
							
							if(Ext.isObject(cmp)) cmp = cmp.id;
							
							componentSet.add(cmp);
						}, this);
					}
				}
			}, this);
			
			delete config.has_many;
		}
		
		Ext.each(this.fields, function(field) {
			//field key			
			var key = field.field;
			
			//add alias for getValue
			if(!Ext.isFunction(this['get'+key])) {
				this['get'+key] = function() {
					return this.getValue(key);
				}
			}
			
			//add alias for getField
			if(!Ext.isFunction(this['get'+key+'Field'])) {
				this['get'+key+'Field'] = function() {
					return this.getField(key);
				}
			}
			
			//add alias for setValue
			if(!Ext.isFunction(this['set'+key])) {
				this['set'+key] = function(value) {
					return this.setValue(key, value);
				}
			}
			
			//add alias for setField
			if(!Ext.isFunction(this['set'+key+'Field'])) {
				this['set'+key+'Field'] = function(field) {
					return this.setField(key, field);
				}
			}
			
			if(this.created) {
				this.$changed.add(key, false);
			}
			else this.$changed.add(key, true);
			
		}, this);
		
		Ext.each(this.has_one, function(has_one) {
			var key = has_one.component+'ID';
			
			if(this.created) {
				this.$changed.add(key, false);
			}
			else this.$changed.add(key, true);
		}, this);
		
		//load provider
		if(Ext.isString(this.provider)) {
			var sections = this.provider.split('.');
			var provider = window;
			
			for(i=0;i<sections.length;i++) {
				provider = provider[sections[i]];
			}
			
			this.provider = provider;
		}
		
		//load api
		if(Ext.isString(this.api)) {
			var sections = this.api.split('.');
			var api = window;
			
			for(i=0;i<sections.length;i++) {
				api = api[sections[i]];
			}
			
			this.api = api;
		}
		
		OB3.DataComponent.superclass.constructor.call(this, config);
	},
	
	initComponent : function() {
		OB3.DataComponent.superclass.initComponent.call(this);
		
		//add events
		this.addEvents(
			/**
			 * @event beforewrite
			 * Fires before the write function is called
			 */
			'beforewrite',
			/**
			 * @event create
			 * Fires when the write request is complete.
			 * @param {Mixed} response
			 */
			'write',
			/**
			 * @event writesuccess
			 * Fires if the write request is successful.
			 * @param {Mixed} response
			 * @param {Ext.Direct.Event} e
			 */
			'writesuccess',
			/**
			 * @event writefailure
			 * Fires if the write request fails
			 * @param {Mixed} response
			 * @param {Ext.Direct.Event} e
			 */
			'writefailure',
			/**
			 * @event afterwrite
			 * Fires after the write function is called
			 */
			'afterwrite',
			/**
			 * @event beforecreate
			 * Fires before the create request is sent.
			 */
			'beforecreate',
			/**
			 * @event create
			 * Fires when the create request is complete.
			 * Returning false will stop the ID being assigned.
			 * @param {Mixed} response
			 */
			'create',
			/**
			 * @event createsuccess
			 * Fires if the create request is successful.
			 * @param {Mixed} response
			 * @param {Ext.Direct.Event} e
			 */
			'createsuccess',
			/**
			 * @event createfailure
			 * Fires if the create request fails
			 * @param {Mixed} response
			 * @param {Ext.Direct.Event} e
			 */
			'createfailure',
			/**
			 * @event afterwrite
			 * Fires after the create function is called
			 */
			'aftercreate',
			/**
			 * @event beforeupdate
			 * Fires before the update request is sent.
			 */
			'beforeupdate',
			/**
			 * @event update
			 * Fires when the update request is complete.
			 * @param {Mixed} response
			 * @param {Ext.Direct.Event} e
			 */
			'update',
			/**
			 * @event updatesuccess
			 * Fires if the update request is successful.
			 * @param {Mixed} response
			 * @param {Ext.Direct.Event} e
			 */
			'updatesuccess',
			/**
			 * @event updatefailure
			 * Fires if the update request fails.
			 * @param {Mixed} response
			 * @param {Ext.Direct.Event} e
			 */
			'updatefailure',
			/**
			 * @event beforedelete
			 * Fires before the delete request is sent.
			 */
			'beforedelete',
			/**
			 * @event delete
			 * Fires when the delete request is complete.
			 * @param {Mixed} response
			 * @param {Ext.Direct.Event} e
			 */
			'delete',
			/**
			 * @event deletesuccess
			 * Fires if the delete request is successful.
			 * @param {Mixed} response
			 * @param {Ext.Direct.Event} e
			 */
			'deletesuccess',
			/**
			 * @event deletefailure
			 * Fires if the delete request fails.
			 * @param {Mixed} response
			 * @param {Ext.Direct.Event} e
			 */
			'deletefailure'
		);
	},
	
	/**
	 * Check to see if a field is defined on this DataComponent
	 * @param {String} key
	 * @return {Boolean}
	 */
	field_defined: function(key) {
		for(var i=0;i<this.fields.length;i++) {
			var field = this.fields[i];
			
			if(field.field == key) {
				return field;
			}
		}
	},
	
	/**
	 * Check to see if a has-one relationship is defined on this DataComponent
	 * @param {String} key
	 * @return {Boolean}
	 */
	has_one_defined: function(key) {
		for(var i=0;i<this.has_one.length;i++) {
			var has_one = this.has_one[i];
			
			if(has_one.field == key) {
				return has_one;
			}
		}
	},
	
	/**
	 * Check to see if a has-many relationship is defined on this DataComponent
	 * @param {String} key
	 * @return {Boolean}
	 */
	has_many_defined: function(key) {
		for(var i=0;i<this.has_many.length;i++) {
			var has_many = this.has_many[i];
			
			if(has_many.field == key) {
				return has_many;
			}
		}
	},
	
	/**
	 * Create a new Field component
	 * @param {String} key
	 * @param {Mixed} value
	 * @return {OB3.Field}
	 */
	createField: function(key, value) {
		for(var i=0;i<this.fields.length;i++) {
			var field = this.fields[i];
			
			var type = field.type; //field type
			var fieldName = field.field; //field name
			var className = type.match(/^([a-z]+)/gi); //classname
			var attributes = type.substring(type.indexOf('(')+1, type.indexOf(')')); //attributes

			if(field.field == key) {
				var newField =  Ext.create({
					name: fieldName,
					value: value,
					attributes: attributes
				}, className);
				
				return newField;
			}
		}
	},
	
	/**
	 * Returns the record collection
	 * @return {Ext.util.MixedCollection}
	 */
	getRecord: function() {
		return this.$record;
	},
	
	/**
	 * Returns the changed collection
	 * @return {Ext.util.MixedCollection}
	 */
	getChanged: function() {
		return this.$changed;
	},
	
	/**
	 * Returns the restore collection
	 * @return {Ext.util.MixedCollection}
	 */
	getRestore: function() {
		return this.$restore;
	},
	
	/**
	 * Returns the original collection
	 * @return {Ext.util.MixedCollection}
	 */
	getOriginal: function() {
		return this.$changed;
	},
	
	/**
	 * @see OB3.Component.setCmp
	 */
	setCmp: function(key, cmp) {
		var cmp = OB3.DataComponent.superclass.setCmp.call(this, key, cmp);
		
		if(cmp) {
			var old = this.getCmp(key);
			
			if(old && old.getValue('ID') !== cmp.getValue('ID')) {
				oldCmp = this.getCmp(key)
			}
		}
	},
	
	/**
	 * Returns the one to one component relationships
	 * @return 
	 */
	getComponents: function() {
		var components = new Ext.util.MixedCollection();
		
		for(var i=0;i<this.has_one.length;i++) {
			var field = this.has_one[i];
			
			if(component = this.getCmp(field.component)) {
				if(component.instanceOf('DataComponent')) {
					components.add(field.field, component);
				}
			}
		}
		
		return components;
	},
	
	/**
	 * Returns the one to one component relationships
	 * @return 
	 */
	getChangedComponents: function() {
		var changed = new Ext.util.MixedCollection();
		
		this.getChanged().eachKey(function(key, value) {
			if(value === true) {
				if(key !== 'ID') key = key.replace('ID', '');
				if(component = this.getCmp(key)) {
					changed.add(key, component);
				}
			}
		}, this);
		
		return changed
	},
	
	/**
	 * Set a field by value on this component.
	 * @param {Mixed} key The key to associate with the field. Or an array of fields
	 * @param {Mixed} value The item to add
	 * @return {Mixed}
	 */
	setValue: function(key, value) {
		if(Ext.isArray(key)) {
			Ext.each(key, function(field) {
				this.setValue(field.field, field.value);
			}, this);
		}
		else if(this.field_defined(key)) {
			if(key === 'ID') this.created = true;
			
			if(Ext.isFunction(this['__set'+key])) {
				value = this['__set'+key](value);
			}	
			
			if(field = this.getField(key)) {
				var old = field.getValue();
				
				if(String(old) !== String(value)) {
					field.setValue(value);
					this.setChanged(key);
				}
			}
			else if(field = this.createField(key, value)) {
				this.setField(key, field);
			}
		}
	},
	
	/**
	 * Set a field on this component.
	 * @param {String} key The key to associate with the field.
	 * @param {Mixed} field The field to add
	 * @return {Mixed}
	 */
	setField: function(key, field) {
		if(this.field_defined(key)) {
			if(key === 'ID') this.created = true;
	
			//apply raw modifer
			if(Ext.isFunction(this['__set'+key+'Field'])) {
				
			}			
	
			if(old = this.getRecord().get(key)) {
				this.getRecord().replace(key, field);
				
				if(key !== 'ID' && old !== field) {
					this.setChanged(key);
				}
			}
			else {
				this.getRecord().add(key, field);
				
				if(key !== 'ID') {
					this.setChanged(key);
				}
			}
		}
	},
	
	/**
	 * Get the value of a field on this component.
	 * @param {String} key The key to look for in the collection.
	 * @return {Mixed}
	 */
	getValue: function(key) {
		if(this.field_defined(key)) {
			if(field = this.getField(key)) {
				var value = field.getValue();

				//apply modifier?
				if(Ext.isFunction(this['__get'+key])) {
					value = this['__get'+key](value);			
				}

				return value;
			}
			//use modifier if no field exists yet
			else if(Ext.isFunction(this['__get'+key])) {
				return this['__get'+key]();	
			}
		}
	},
	
	/**
	 * Get a field on this component.
	 * @param {String} key The key to look for in the collection.
	 * @return {Mixed}
	 */
	getField: function(key) {
		if(this.field_defined(key)) {		
			var field = this.getRecord().get(key)
			
			//apply modifier?
			if(Ext.isFunction(this['__get'+key+'Field'])) {
				field = this['__get'+key+'Field'](field);
			}
			
			return field;
		}
	},
	
	/**
	 * Check if a field exists on this component.
	 * @param {String} key The key to look for in the collection.
	 * @return {Mixed}
	 */
	hasField: function(key) {
		return this.getRecord().containsKey(key);
	},
	
	/**
	 * Get fields marked as changed
	 * @return {Mixed}
	 */
	getFields: function() {
		var fields = new Ext.util.MixedCollection();
		
		for(var i=0;i<this.fields.length;i++) {
			var field = this.fields[i];
			
			if(field = this.getField(field.field)) {
				components.add(field.field, field);
			}
		}
		
		return components;
	},
	
	/**
	 * Get fields marked as changed
	 * @return {Mixed}
	 */
	getChangedFields: function() {
		var changed = new Ext.util.MixedCollection();
		
		this.getChanged().eachKey(function(key, value) {
			if(value === true) {
				if(field = this.getField(key)) {
					changed.add(key, field);
				}
			}
		}, this);
		
		return changed;
	},
	
	/**
	 * Mark a field as changed.
	 * @param {String} key The key associated with the field.
	 * @return {Mixed}
	 */
	setChanged: function(key) {
		if(this.getChanged().get(key)) {
			this.getChanged().replace(key, true);
		}
		else this.getChanged().add(key, true);
	},
	
	/**
	 * Check to see if the component has been created
	 * @return {Boolean}
	 */
	isCreated: function() {
		if(this.getValue('ID') && this.created) {
			return true;
		}
		else return false;
	},
	
	/**
	 * Check to see if the component is still creating
	 * @return {Boolean}
	 */
	isCreating: function() {
		return (this.creating && !this.isCreated());
	},
	
	setJoin: function() {
		this.setValue('ID', OB3.util.uniqid());
		
		this.created = false;
		this.join = true;
	},
	
	canJoin: function() {
		return this.allowJoin;
	},
	
	hasJoin: function() {
		return this.join;
	},
	
	/**
	 * Check if the component is empty, eg. does not have any records set.
	 * @return {Boolean}
	 */
	isEmpty: function() {
		var isEmpty = true;
		
		this.getRecord().eachKey(function(key, item) {
			if(key !== 'ID' && item) {
				isEmpty = isEmpty && item.isEmpty();
			}
		}, this);
		
		return isEmpty;
	},
	
	/**
	 * Check if the component has been changed since last write.
	 * @return {Boolean}
	 */
	isChanged: function() {
		var isChanged = false;
		
		this.getChanged().eachKey(function(key, item) {
			if(key !== 'ID' && item === true) {
				isChanged = true;
			}
		}, this);
		
		return isChanged;
	},
	
	/**
	 * Mark all fields as changed
	 */
	forceChanges: function(fields) {
		this.getChanged().eachKey(function(key, item) {
			if(Ext.isArray(fields)) {
				if(fields.indexOf(key) >= 0) {
					this.getChanged().replace(key, true);
				}
			}
			else if(key !== 'ID') {
				this.getChanged().replace(key, true);
			}
		}, this);
	},
	
	/**
	 * Clear all changes
	 */
	clearChanges: function() {
		this.getChanged().eachKey(function(key, item) {
			this.getRestore().replace(key, item);
			
			if(key !== 'ID') {
				this.getChanged().replace(key, false);
			}
		}, this);
	},
	
	/**
	 * Restore previous changes
	 */
	restoreChanges: function() {
		this.getRestore().eachKey(function(key, item) {
			if(key !== 'ID') {
				this.getChanged().replace(key, item);
			}
		}, this);
	},
	
	/**
	 * Get parent one to one relationship
	 * @return {OB3.Component}
	 */
	getParent: function() {
		for(i=0;i<this.has_one.length;i++) {
			var has_one = this.has_one[i];
			
			if(has_one.parent) {
				var component = this.getCmp(has_one.field);
				
				if(component && component.instanceOf('DataComponent')) {
					return component;
				}
				else return false;
			}
		}
	},
	
	getParentFieldName: function() {
		for(i=0;i<this.has_one.length;i++) {
			var has_one = this.has_one[i];
			
			if(has_one.parent) {
				return has_one.field+'ID';
			}
		}
	},
	
	/**
	 * Get previous one to one relationship
	 * @return {OB3.Component}
	 */
	getPrevious: function() {
		for(i=0;i<this.has_one.length;i++) {
			var has_one = this.has_one[i];
			
			if(has_one.previous) {
				var component = this.getCmp(has_one.field);
				
				if(component && component.instanceOf('DataComponent')) {
					return component;
				}
				else return false;
			}
		}
	},
	
	getPreviousFieldName: function() {
		for(i=0;i<this.has_one.length;i++) {
			var has_one = this.has_one[i];
			
			if(has_one.previous) {
				return has_one.field+'ID';
			}
		}
	},
	
	/**
	 * Get the api thread.
	 * Use parent thread if it exists.
	 */
	getThread: function() {
		if(this.getParent()) {
			return this.getParent().getId();
		}
		else return this.getId();
	},
	
	/**
	 * Get all fields to be sent including component IDs
	 */
	getDataFields: function() {
		//create data to send
		var data = new Object();
		
		if(this.isCreated()) {
			data['ID'] = this.getValue('ID');
		}
		
		data['ClassName'] = this.className;
		
		//add component relationships
		this.getChangedComponents().eachKey(function(key, component) {
			if(component.isCreated() || component.hasJoin()) {
				data[key+'ID'] = component.getValue('ID');
			}
		}, this);
		
		//add changed fields
		this.getChangedFields().eachKey(function(key, field) {
			data[key] = field.getValue();
		}, this);
		
		return data;
	},
	
	/**
	 * Get only component fields to be sent.
	 */
	getComponentFields: function() {
		//create data to send
		var data = new Object();
		
		if(this.isCreated()) {
			data['ID'] = this.getValue('ID');
		}
		
		data['ClassName'] = this.className;
		
		//add component relationships
		this.getChangedComponents().eachKey(function(key, component) {
			if(component.isCreated() || component.hasJoin()) {
				data[key+'ID'] = component.getValue('ID');
			}
		}, this);
		
		return data;
	},
	
	/**
	 * Return if the component is sequentially queued for creation.
	 */
	isQueued: function() {
		return this.queued;
	},
	
	/**
	 * Set component as queued
	 */
	setQueued: function() {
		this.queued = true;
	},
	
	/**
	 * Write changes via api
	 * @param {Boolean} forceWrite force write
	 * @param {Boolean} forceCreate force create/recreate
	 * @param {Boolean} writeComponents force write on components, if true or 1
	 * the direct components of this components will be called, if 2 it will bubble
	 * up through the related components.
	 */
	write: function(forceWrite, forceCreate, writeComponents, args) {
		if(writeComponents === true || writeComponents === 1) {
			this.writeComponents(forceWrite, forceCreate, null, args); //force write on components
		}
		else if(writeComponents === 2) {
			this.writeComponents(forceWrite, forceCreate, writeComponents, args); //bubble write on components
		}
		
		if(forceWrite) this.forceChanges(forceWrite);
		
		if((this.isCreated() || this.isCreating()) && !forceCreate) {
			if(this.isCreating()) {
				this.on('createsuccess', function() { this.write(forceWrite, forceCreate, null, args); }, this);
			}
			else if((this.isChanged() && !this.isEmpty()) || forceWrite) {
				if(this.fireEvent('beforewrite') !== false) {
					var data = this.getDataFields(); //get data fields
					if(args) data['@args'] = args; //apply additional arguments
					
					this.api.write.thread(OB3.util.uniqid(), data, function(response, e) {
						if(this.fireEvent('write', response, e) !== false) {
							if(e.type == 'exception') {
								this.restoreChanges(); //restore changes
								
								this.fireEvent('writefailure', response, e);
							}
							else if(this.fireEvent('writesuccess', response, e) !== false) {
								//
							}
						}
					}, this);
					
					this.clearChanges(); //clear changes
					
					this.fireEvent('afterwrite');
				}
			}
		}
		else {
			var thread = this.getThread(); //thread
			
			if(
				this.getParent() && 
				this.getParent().isCreated() === false &&
				this.getParent().isCreating() === false
			) {
				if(this.getParent().canJoin()) {
					this.getParent().setJoin(); //set a join
					
					var args = Ext.apply({
						join: [this.getParentFieldName()]
					}, args);
					
					this.getParent().on('aftercreate', function() { this.write(forceWrite, forceCreate, null, args); }, this);
				}
				else this.getParent().on('createsuccess', function() { this.write(forceWrite, forceCreate, null, args); }, this);
			}
			else if(
				this.provider.isQueued(thread) === true &&
				this.getPrevious() && 
				this.getPrevious().isCreated() === false &&
				this.isQueued() === false
			) {
				if(this.getPrevious().isQueued() || this.getPrevious().isCreating()) {
					this.setQueued();
				}
				
				if(!OB3.DataComponent.getQueue(thread)) {
					OB3.DataComponent.setQueue(thread, this.getPrevious());
				}
				
				OB3.DataComponent.clearQueue.delay(100, null, null, [thread]);
				
				OB3.DataComponent.getQueue(thread).on('createsuccess', function() { this.write(forceWrite, forceCreate, null, args); }, this);
			}
			else if(
				this.getPrevious() && 
				this.getPrevious().isCreated() === false &&
				this.isQueued() === false
			) {
				if(this.getPrevious().canJoin()) {
					this.getPrevious().setJoin(); //set a join
					
					var args = Ext.apply({
						join: [this.getParentFieldName()]
					}, args);
					
					this.getPrevious().on('aftercreate', function() { this.write(forceWrite, forceCreate, null, args); }, this);
				}
				else this.getPrevious().on('createsuccess', function() { this.write(forceWrite, forceCreate, null, args); }, this);
			}
			else {
				if(this.fireEvent('beforecreate') !== false && this.fireEvent('beforewrite') !== false) {
					var data = this.getDataFields(); //get data fields
					if(args) data['@args'] = args; //apply additional arguments
					
					this.api.create.thread(thread, data, function(response, e) {
						if(this.fireEvent('create', response, e) !== false && this.fireEvent('write', response, e) !== false) {
							if(e.type == 'exception') {
								this.restoreChanges(); //restore changes
								
								this.fireEvent('writefailure', response, e);
							}
							else if(this.fireEvent('writesuccess', response, e) !== false) {
								if(Ext.isNumber(response.ID)) {
									this.setValue('ID', response.ID);
									
									this.fireEvent('createsuccess', response, e);
								}
								else this.fireEvent('createfailure', response, e)
							}
						}
					}, this);
					
					this.creating = true;
					
					this.clearChanges(); //clear changes
					
					this.fireEvent('afterwrite');
					this.fireEvent('aftercreate');
				}
			}
		}
	},
	
	/**
	 * Alias to write.
	 */
	forceWrite: function(args) {
		this.write(true, null, null, args);
	},
	
	/**
	 * Alias to write.
	 */
	forceCreate: function(args) {
		this.write(null, true, null, args);
	},
	
	/**
	 * Calls write on all related components
	 * @param {Boolean} forceWrite force write
	 * @param {Boolean} forceCreate force create/recreate
	 * @param {Boolean} writeComponents force write on components of components
	 */
	writeComponents: function(forceWrite, forceCreate, writeComponents, args) {
		this.getComponents().eachKey(function(key, component) {
			if(Ext.isFunction(component.write)) {
				component.write(forceWrite, forceCreate, writeComponents, args);
			}
		}, this);
	},
	
	/**
	 * Calls write but delays it x (ms) and cancels any previous delayed writes.
	 * @param {Int} delay Amount of time in ms to delay the write (defaults to 1000)
	 * @param {Boolean} forceWrite force write
	 * @param {Boolean} forceCreate force create/recreate
	 * @param {Boolean} writeComponents force write on components of components
	 */
	delayedWrite: function(delay, forceWrite, forceCreate, writeComponents, args) {
		this.delayedWriteTask.cancel(); //cancel previous so we don't double up.
		this.delayedWriteTask.delay(delay ? delay : 1000, null, this, [forceWrite, forceCreate, writeComponents, args]);
	},
	
	//for above delayed write
	delayedWriteTask: new Ext.util.DelayedTask(function(forceWrite, forceCreate, writeComponents, args) {
		this.write(forceWrite, forceCreate, writeComponents, args);
	}),

	/**
	 * Update this component via api.
	 * @param {Boolean} updateComponents force update on components, if true or 1
	 * the direct components of this components will be called, if 2 it will bubble
	 * up through the related components.
	 * @param {Array} args Additional non component related arguments
	 */
	update: function(updateComponents, args) {
		if(this.fireEvent('beforeupdate') !== false) {
			if(updateComponents === true || updateComponents === 1) {
				this.updateComponents(); //force update on components
			}
			else if(updateComponents === 2) {
				this.updateComponents(updateComponents); //bubble update on components
			}
			
			if(this.isCreated() || this.isCreating()) {
				if(this.isCreating()) {
					this.on('createsuccess', function() { this.update(updateComponents) }, this);
				}
				else {
					var data = this.getComponentFields();
					if(args) data['@args'] = args; //apply additional arguments
					
					this.api.update.thread(this.getThread(), data, function(response, e) {
						if(this.fireEvent('update', response, e) !== false) {
							if(e.type == 'exception') {
								this.fireEvent('updatefailure', response, e);
							}
							else if(this.fireEvent('updatesuccess', response, e) !== false) {
								if(Ext.isArray(response)) {
									//Do I get run??! Variable "respone" is an associative array. 
									Ext.each(response, function(item) {
										this.setValue(item.field, item.value);
									}, this);
								}
							}
						}
					}, this);
				}
			}
		}
	},
	
	/**
	 * Calls update on all related components.
	 * @param {Boolean} updateComponents force update on components of components
	 */
	updateComponents: function(updateComponents, args) {
		this.getComponents().eachKey(function(key, component) {
			if(Ext.isFunction(component.update)) {
				component.update(updateComponents, args);
			}
		}, this);
	},
	
	/**
	 * Delete this data component
	 * @param {Boolean} updateComponents force delete on components, if true or 1
	 * the direct components of this components will be called, if 2 it will bubble
	 * up through all related components.
	 */
	delete: function(deleteComponents, args) {
		if(deleteComponents === true || deleteComponents === 1) {
			this.deleteComponents(); //force update on components
		}
		else if(deleteComponents === 2) {
			this.deleteComponents(deleteComponents); //bubble update on components
		}
		
		if(this.isCreated() || this.isCreating()) {
			if(this.isCreating()) {
				this.on('createsuccess', function() { this.delete(deleteComponents); }, this);
			}
			else if(this.fireEvent('beforedelete') !== false) {
				var data = {ID: this.getValue('ID')};
				if(args) data['@args'] = args; //apply additional arguments
				
				this.api.delete.thread(this.getThread(), data, function(response, e) {
					if(this.fireEvent('delete', response, e) !== false) {
						if(e.type == 'exception') {
							this.fireEvent('deletefailure', response, e);
						}
						else if(this.fireEvent('deletesuccess', response, e) !== false) {
							//
						}
					}
				}, this);
				
				this.fireEvent('afterdelete');
			}
		}
	},
	
	/**
	 * Calls delete on all related components.
	 * @param {Boolean} deleteComponents force delete on components of components
	 */
	deleteComponents: function(deleteComponents, args) {
		this.getComponents().eachKey(function(key, component) {
			if(Ext.isFunction(component.delete)) {
				component.update(deleteComponents, args);
			}
		}, this);
	}
	
});

/**
 * Static Functions
 */
Ext.apply(OB3.DataComponent, {
	
	/**
	 * List of active threads and there last queued component. This is used for
	 * sequential creation optimization.
	 * @static {Object} $threads
	 */
	$queue: {},
	
	/**
	 * Get the last queued component for a thread.
	 * @param {String} thread
	 */
	getQueue: function(thread) {
		return Ext.getCmp(OB3.DataComponent.$queue[thread]);
	},
	
	/**
	 * Set the last queued component for a thread.
	 * @param {String} thread
	 * @param {Mixed} o 
	 */
	setQueue: function(thread, o) {
		if(Ext.isObject(o)) {
			o = o.id;
		}
		
		OB3.DataComponent.$queue[thread] = o;
	},
	
	/**
	 * {DelayedTask} Clear the last queued component from a thread at a delayed 
	 * time.
	 * @param {String} thread
	 */
	clearQueue: new Ext.util.DelayedTask(function(thread) {
		OB3.DataComponent.$queue[thread] = null;
	})
	
});

Ext.reg('OB3.DataComponent', OB3.DataComponent);